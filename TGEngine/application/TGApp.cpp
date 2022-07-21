#include "TGAppGUI.hpp"
#include <IO/IOModule.hpp>
#include <TGEngine.hpp>
#include <Util.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <graphics/GameGraphicsModule.hpp>
#include <graphics/vulkan/VulkanModuleDef.hpp>
#include <graphics/vulkan/VulkanShaderModule.hpp>
#include <graphics/vulkan/VulkanShaderPipe.hpp>
#undef ERROR
#define SPR_NO_GLSL_INCLUDE 1
#define SPR_NO_STATIC 1
#define SPR_STATIC extern
#include "TGAppIO.hpp"
#include <Util.hpp>
#include <array>
#include <cmath>
#include <headerlibs/ShaderPermute.hpp>
#include <iostream>
#include <limits>
#include <regex>
#include <string>
#include "Calculations.hpp"
#undef min
#undef max

using namespace tge::main;
using namespace tge::graphics;
using namespace tge;

namespace permute {
std::map<std::string, int> lookupCounter;
permute::lookup glslLookup = {{"next", next}};
} // namespace permute

TGAppGUI *guiModul = new TGAppGUI;
TGAppIO *ioModul = new TGAppIO;


inline std::tuple<uint32_t, uint32_t>
createShaderPipes(tge::graphics::VulkanGraphicsModule *api,
                  tge::shader::VulkanShaderModule *shader) {

  glslang::InitializeProcess();
  util::OnExit exitHandle(&glslang::FinalizeProcess);
  shader::VulkanShaderPipe shaderPipe{};

  auto fragment = permute::fromFile<permute::PermuteGLSL>("assets/test.frag");
  if (!fragment.generate()) {
    for (auto &str : fragment.getContent())
      printf(str.c_str());
    return {-1, -1};
  }

  auto perm = permute::fromFile<permute::PermuteGLSL>(
      "assets/perInstanceVertexShader.vert");
  std::array<Material, MAX_DEGREE> material;
  const auto beginShader = shader->pipeInfos.size();
  for (size_t i = 0; i < MAX_DEGREE; i++) {
    permute::glslLookup["degree"] = [&](const auto &input) {
      return std::to_string(i);
    };
    permute::glslLookup["steps"] = permute::glslLookup["degree"];
    permute::glslLookup["allpoints"] = [&](const auto &input) {
      return std::to_string(std::max(cellDataPerLayer[i].size(), (size_t)1));
    };
    if (!perm.generate({std::to_string(i)})) {
      for (auto &str : perm.getContent())
        printf(str.c_str());
      return {-1, -1};
    }
    auto shaderpipe = new tge::shader::VulkanShaderPipe;
    shaderpipe->inputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo();

    shaderpipe->shader.push_back(
        {fragment.getBinary(), vk::ShaderStageFlagBits::eFragment});
    shaderpipe->shader.push_back(
        {perm.getBinary(), vk::ShaderStageFlagBits::eVertex});

    shaderpipe->descriptorLayoutBindings = {
        DescriptorSetLayoutBinding(0, vk::DescriptorType::eStorageBuffer, 1,
                                   vk::ShaderStageFlagBits::eVertex),
        DescriptorSetLayoutBinding(1, vk::DescriptorType::eUniformBuffer, 1,
                                   vk::ShaderStageFlagBits::eVertex)};

    const vk::DescriptorSetLayoutCreateInfo layoutCreate(
        {}, shaderpipe->descriptorLayoutBindings);
    const auto descLayout = api->device.createDescriptorSetLayout(layoutCreate);
    shader->setLayouts.push_back(descLayout);

    std::vector<vk::DescriptorPoolSize> descPoolSizes;
    for (const auto &binding : shaderpipe->descriptorLayoutBindings) {
      descPoolSizes.push_back(
          {binding.descriptorType, binding.descriptorCount});
    }
    const vk::DescriptorPoolCreateInfo descPoolCreateInfo({}, 1000,
                                                          descPoolSizes);
    const auto descPool = api->device.createDescriptorPool(descPoolCreateInfo);
    shader->descPools.push_back(descPool);

    const auto layoutCreateInfo = vk::PipelineLayoutCreateInfo({}, descLayout);
    const auto pipeLayout = api->device.createPipelineLayout(layoutCreateInfo);
    shader->pipeLayouts.push_back(pipeLayout);
    shaderpipe->layoutID = shader->pipeLayouts.size() - 1;
    material[i].costumShaderData = shaderpipe;
    material[i].primitiveType = (uint32_t)PrimitiveTopology::eTriangleFan;
    shaderpipe->needsDefaultBindings = false;
    shader->createBindings(shaderpipe, 1);
  }
  glslang::FinalizeProcess();

  return {api->pushMaterials(material.size(), material.data()), beginShader};
}

inline uint32_t createBuffer(tge::graphics::VulkanGraphicsModule *api,
                             tge::shader::VulkanShaderModule *shader,
                             uint32_t materialID, uint32_t shaderOffset) {
  std::vector<size_t> sizes = {sizeof(ioModul->mvpMatrix)};
  std::vector<void *> data = {&ioModul->mvpMatrix};
  std::vector<size_t> layer = {};
  for (size_t i = 0; i < MAX_DEGREE; i++) {
    if (cellDataPerLayer[i].empty())
      continue;
    layer.push_back(i);
    sizes.push_back(cellDataPerLayer[i].size() *
                    sizeof(cellDataPerLayer[i][0]));
    data.push_back(cellDataPerLayer[i].data());
  }

  const auto bufferID =
      api->pushData(data.size(), data.data(), sizes.data(), DataType::All);

  std::vector<RenderInfo> actualInfos;
  std::vector<shader::BindingInfo> infos;
  actualInfos.reserve(MAX_DEGREE);
  infos.reserve(MAX_DEGREE * 2);
  for (size_t i = 0; i < layer.size(); i++) {
    const size_t cLayer = layer[i];

    shader::BindingInfo info;
    info.binding = 0;
    info.bindingSet = shader->pipeInfos[shaderOffset + i].descSet;
    info.type = shader::BindingType::Storage;
    info.data.buffer.dataID = bufferID + i + 1;
    info.data.buffer.offset = 0;
    info.data.buffer.size = sizes[i + 1];
    infos.push_back(info);

    info.binding = 1;
    info.type = shader::BindingType::UniformBuffer;
    info.data.buffer.dataID = bufferID;
    info.data.buffer.size = sizes[i];
    infos.push_back(info);

    RenderInfo renderInfo;
    renderInfo.indexSize = IndexSize::NONE;
    renderInfo.materialId = materialID + i;
    renderInfo.firstInstance = 0;
    renderInfo.indexCount = 4;
    renderInfo.instanceCount = cellDataPerLayer[cLayer].size() / 4;
    renderInfo.bindingID = shaderOffset + i;
    actualInfos.push_back(renderInfo);
  }
  shader->bindData(infos.data(), infos.size());
  api->pushRender(actualInfos.size(), actualInfos.data());
  return bufferID;
}

inline void readData(const std::string &&input) {
  std::ifstream fstream(input, std::ios_base::binary);
  Cell cell;
  std::regex findNumbers = std::regex("([^;,]*)[;,]*");
  std::array<float, 4> numbers;
  uint8_t index = 0;
  while (fstream.good() && !fstream.eof()) {
    std::string line;
    std::getline(fstream, line);
    const char control = line[0];
    std::smatch matches;
    uint8_t numberIndex = 0;
    while (std::regex_search(line, matches, findNumbers,
                             std::regex_constants::match_any)) {
      if (numberIndex != 0) {
        const std::string numberString = matches[1].str();
        if (numberString.empty())
          break;
        const auto number = std::stof(numberString);
        numbers[numberIndex - 1] = number;
      }
      numberIndex++;
      line = matches.suffix().str();
    }
    switch (control) {
    case 'M':
      if (!cell.polynomials.empty()) {
        const uint32_t degree =
            std::round(std::pow(cell.polynomials.size(), 1 / 3.0f));
        cellsPerLayer[degree].push_back(cell);
      }
      index = 0;
      cell = {};
      cell.centerOfCell = glm::vec3(numbers[0], numbers[1], numbers[2]);
      break;
    case 'P':
      cell.points[index++] = glm::vec3(numbers[0], numbers[1], numbers[2]);
      break;
    case 'V':
      cell.polynomials.push_back(
          glm::vec4(numbers[0], numbers[1], numbers[2], numbers[3]));
      break;
    default:
      break;
    }
  }
  const uint32_t degree =
      std::round(std::pow(cell.polynomials.size(), 1 / 3.0f));
  cellsPerLayer[degree].push_back(cell);
}



int main() {
  lateModules.push_back(guiModul);
  lateModules.push_back(ioModul);

  const auto initResult = init();
  if (initResult != main::Error::NONE) {
    printf("Error in init!");
    return -1;
  }

  auto api = (tge::graphics::VulkanGraphicsModule *)getAPILayer();
  auto shader = (tge::shader::VulkanShaderModule *)api->getShaderAPI();
  ioModul->api = api;

  readData("degree5.dcplt");
  makeData(guiModul->currentY,  guiModul->interpolation);

  const auto [materialPoolID, shaderOffset] = createShaderPipes(api, shader);
  const auto bufferPoolID =
      createBuffer(api, shader, materialPoolID, shaderOffset);
  ioModul->binding = bufferPoolID;
  ioModul->sendChanges();

  Light light;
  light.color = glm::vec3(1, 1, 1);
  light.pos = glm::vec3(0, 100, 0);
  light.intensity = 100.0f;
  api->pushLights(1, &light);

  const auto startResult = start();
  if (startResult != main::Error::NONE) {
    printf("Error in start!");
    return -1;
  }

  return (uint32_t)startResult;
}