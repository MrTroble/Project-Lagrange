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
#undef min
#undef max

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
std::array<std::vector<glm::vec4>, MAX_DEGREE> cellDataPerLayer;
glm::mat4 mvpMatrix(1);
TGAppGUI *guiModul = new TGAppGUI;

constexpr std::array<uint32_t, MAX_DEGREE> indexCount = {4,  4,  8, 12,
                                                         16, 20, 24};

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
  std::ifstream fstream(input, std::ios_base::binary);
  char control = 'M';
  std::stringstream stream;
  std::vector<float> floating;
  Cell cell;
  uint32_t index = 0;
  cell.polynomials.reserve(120);
  while (!fstream.eof()) {
    char nextChar = fstream.get();
    if (nextChar == '\r')
      continue;
    if (nextChar == ';') {
      stream = {};
      continue;
    }
    if (nextChar == ',' || nextChar == '\n') {
      const auto string = stream.str();
      floating.push_back(std::stof(string));
      stream = {};
      if (nextChar == '\n') {
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
        stream = {};
        control = '_';
      }
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
  const uint32_t degree = std::floor(std::pow(cell.polynomials.size(), 1 / 3.0f));
  cellsPerLayer[degree].push_back(cell);
}

inline void makeData() {
  std::numeric_limits<float> flim;
  float currentY = guiModul->currentY;
  for (size_t i = 0; i < indexCount.size(); i++) {
    const auto &cLayer = cellsPerLayer[i];
    for (const auto &cell : cLayer) {
      glm::vec3 maxVec(flim.min(), flim.min(), flim.min());
      glm::vec3 minVec(flim.max(), flim.max(), flim.max());
      for (size_t z = 0; z < 8; z++) {
        for (size_t id = 0; id < 3; id++) {
          const auto current = cell.points[z][id];
          maxVec[id] = std::max(maxVec[id], current);
          minVec[id] = std::min(minVec[id], current);
        }
      }
      if (!(maxVec.y >= currentY && currentY >= minVec.y))
        continue;
      const auto maxSize = indexCount[i];
      auto &data = cellDataPerLayer[i];
      const auto start = data.size();
      data.resize(start + maxSize);
      if (maxSize == 4) {
        data[start + 0] = glm::vec4(minVec.x, minVec.y, 0, 1);
        data[start + 1] = glm::vec4(maxVec.x, minVec.y, 0, 1);
        data[start + 2] = glm::vec4(maxVec.x, maxVec.y, 0, 1);
        data[start + 3] = glm::vec4(minVec.x, maxVec.y, 0, 1);
      } else {
        const double side = std::sqrt(maxSize);
        const glm::vec2 difference = glm::vec2(maxVec - minVec);
        for (size_t x = 0; x < maxSize; x++) {
          const double xInterpolation = (x % (int)side) / side;
          const double yInterpolation = std::floor(x / side);
          const auto position =
              difference * glm::vec2(xInterpolation, yInterpolation) +
              glm::vec2(minVec);
          data[start + x] = glm::vec4(position, 0, 1);
        }
      }
    }
  }
}

int main() {
  mvpMatrix = glm::perspective(glm::radians(75.0f), 1.0f, 0.0001f, 10000.0f);
  lateModules.push_back(guiModul);

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