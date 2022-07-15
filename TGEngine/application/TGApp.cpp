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
#include <Util.hpp>
#include <array>
#include <cmath>
#include <headerlibs/ShaderPermute.hpp>
#include <limits>

using namespace tge::main;
using namespace tge::graphics;
using namespace tge;

namespace permute {
std::map<std::string, int> lookupCounter;
permute::lookup glslLookup = {{"next", next}};
} // namespace permute

constexpr auto MAX_DEGREE = 7;

struct Cell {
  glm::vec3 centerOfCell;
  glm::vec3 points[8];
  std::vector<glm::vec4> polynomials;
};

size_t actualInUse = 0;
std::array<std::vector<Cell>, MAX_DEGREE> cellsPerLayer;
std::array<std::vector<float>, MAX_DEGREE> cellDataPerLayer;
glm::mat4 mvpMatrix(1);
constexpr std::array<uint32_t, MAX_DEGREE> indexCount = {4,  4,  8, 12,
                                                         16, 20, 24};

float currentY = 0;

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
    permute::glslLookup["allpoints"] = [&](const auto &input) { return "4"; };
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
    shaderpipe->needsDefaultBindings = false;
    shader->createBindings(shaderpipe, 1);
  }
  glslang::FinalizeProcess();

  return {api->pushMaterials(material.size(), material.data()), beginShader};
}

inline uint32_t createBuffer(tge::graphics::VulkanGraphicsModule *api,
                             tge::shader::VulkanShaderModule *shader,
                             uint32_t materialID, uint32_t shaderOffset) {
  std::vector<size_t> sizes = {sizeof(mvpMatrix)};
  std::vector<void *> data = {&mvpMatrix};
  std::vector<size_t> layer = {};
  for (size_t i = 0; i < MAX_DEGREE; i++) {
    if (cellDataPerLayer[i].empty())
      continue;
    layer.push_back(i);
    sizes.push_back(cellDataPerLayer[i].size());
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
    renderInfo.indexCount = indexCount[cLayer];
    renderInfo.instanceCount =
        cellDataPerLayer[cLayer].size() / renderInfo.indexCount;
    renderInfo.bindingID = shaderOffset + i;
    actualInfos.push_back(renderInfo);
  }
  shader->bindData(infos.data(), infos.size());
  api->pushRender(actualInfos.size(), actualInfos.data());
  return bufferID;
}

inline void readData(std::string &&input) {
  std::ifstream fstream(input);
  char control;
  std::stringstream stream;
  std::vector<float> floating;
  Cell cell;
  uint32_t index = 0;
  cell.polynomials.reserve(120);
  while (!fstream.eof()) {
    char nextChar;
    fstream >> nextChar;
    if (nextChar == ';')
      continue;
    if (nextChar == ',') {
      floating.push_back(std::atof(stream.str().c_str()));
      stream.clear();
    } else if (nextChar == '\n') {
      switch (control) {
      case 'M':
        cell.centerOfCell = glm::vec3(floating[0], floating[1], floating[2]);
        break;
      case 'P':
        cell.points[index] = glm::vec3(floating[0], floating[1], floating[2]);
        index++;
        break;
      case 'V':
        cell.polynomials.push_back(
            glm::vec4(floating[0], floating[1], floating[2], floating[3]));
      default:
        break;
      }
      floating.clear();
      control = '_';
    } else if (control == '_') {
      control = nextChar;
      if (control == 'M') {
        const uint32_t degree = std::pow(cell.polynomials.size(), 1 / 3);
        cellsPerLayer[degree].push_back(cell);
        cell = {};
      }
    } else {
      stream << nextChar;
    }
  }
  const uint32_t degree = std::pow(cell.polynomials.size(), 1 / 3);
  cellsPerLayer[degree].push_back(cell);
}

#undef min
#undef max

inline void makeData() {
  std::numeric_limits<float> flim;
  for (size_t i = 0; i < indexCount.size(); i++) {
    const auto &cLayer = cellsPerLayer[i];
    for (const auto &cell : cLayer) {
      float ymax = flim.max();
      float ymin = flim.min();
      for (size_t z = 0; z < 8; z++) {
        ymax = std::max(ymax, cell.points[z].y);
        ymin = std::min(ymin, cell.points[z].y);
      }
      if (!(ymax >= currentY && currentY >= ymin))
        continue;
      for (size_t x = 0; x < indexCount[i]; x++) {

      }
    }
  }
}

int main() {
  TGAppGUI *gui = new TGAppGUI;
  lateModules.push_back(gui);

  const auto initResult = init();
  if (initResult != main::Error::NONE) {
    printf("Error in init!");
    return -1;
  }

  auto api = (tge::graphics::VulkanGraphicsModule *)getAPILayer();
  auto shader = (tge::shader::VulkanShaderModule *)api->getShaderAPI();

  readData("testInput.txt");
  makeData();

  const auto [materialPoolID, shaderOffset] = createShaderPipes(api, shader);
  const auto bufferPoolID =
      createBuffer(api, shader, materialPoolID, shaderOffset);

  const auto startResult = start();
  if (startResult != main::Error::NONE) {
    printf("Error in start!");
    return -1;
  }

  return (uint32_t)startResult;
}