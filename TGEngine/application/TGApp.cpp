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
#include <headerlibs/ShaderPermute.hpp>

using namespace tge::main;
using namespace tge::graphics;
using namespace tge;

namespace permute {
std::map<std::string, int> lookupCounter;
permute::lookup glslLookup = {{"next", next}};
} // namespace permute

constexpr auto MAX_DEGREE = 7;

struct Cell {
  glm::vec3 points[8];
};

std::array<Cell, 17483648> cells;

inline std::tuple<uint32_t, uint32_t>
createShaderPipes(tge::graphics::VulkanGraphicsModule *api,
                  tge::shader::VulkanShaderModule *shader) {

  glslang::InitializeProcess();
  util::OnExit exitHandle(&glslang::FinalizeProcess);
  shader::VulkanShaderPipe shaderPipe{};
  auto perm = permute::fromFile<permute::PermuteGLSL>(
      "assets/perInstanceVertexShader.vert");
  std::array<Material, MAX_DEGREE> material;
  const auto beginShader = shader->pipeInfos.size();
  for (size_t i = 0; i < MAX_DEGREE; i++) {
    permute::glslLookup["degree"] = [&](const auto &input) {
      return std::to_string(i);
    };
    permute::glslLookup["steps"] = permute::glslLookup["degree"];
    if (!perm.generate({std::to_string(i)})) {
      for (auto &str : perm.getContent())
        printf(str.c_str());
      return {-1, -1};
    }
    auto shaderpipe = new tge::shader::VulkanShaderPipe;
    shaderpipe->inputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo();

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
                             uint32_t materialID, uint32_t shaderOffset,
                             std::array<uint32_t, MAX_DEGREE> &sizePerDegree) {
  std::array<std::vector<uint32_t>, MAX_DEGREE> indexBuffer;
  std::array<RenderInfo, MAX_DEGREE> renderInfos;

  for (size_t i = 0; i < MAX_DEGREE; i++) {
    std::vector<uint32_t> indices;
    if (i < 2) {
      indices.resize(6);
    } else {
      indices.resize(i * i * i * i * 3 / 2);
    }
    for (size_t x = 0; x < indices.size() / 6; x++) {
      indices[x] = x;
      indices[x + 1] = x + 1;
      indices[x + 2] = x + 2;
      indices[x + 3] = x;
      indices[x + 4] = x + 2;
      indices[x + 5] = x + 3;
    }
    indexBuffer[i] = indices;
    renderInfos[i].indexCount = indices.size();
  }

  std::vector<size_t> sizes = {cells.size() * sizeof(Cell)};
  std::vector<void *> data = {cells.data()};
  sizes.reserve(MAX_DEGREE + 1);
  data.reserve(MAX_DEGREE + 1);
  for (const auto &buffer : indexBuffer) {
    sizes.push_back(buffer.size() * sizeof(uint32_t));
    data.push_back((void *)buffer.data());
  }

  const auto bufferID =
      api->pushData(data.size(), data.data(), sizes.data(), DataType::All);

  size_t offset = 0;

  std::vector<RenderInfo> actualInfos;
  std::vector<shader::BindingInfo> infos;
  actualInfos.reserve(MAX_DEGREE);
  for (size_t i = 0; i < MAX_DEGREE; i++) {
    shader::BindingInfo info;
    info.binding = 0;
    info.bindingSet = shader->pipeInfos[shaderOffset + i].descSet;
    info.type = shader::BindingType::Storage;
    info.data.buffer.dataID = bufferID;
    info.data.buffer.offset = 0;
    info.data.buffer.size = sizes[0];
    infos.push_back(info);

    if (sizePerDegree[i] < 1)
      continue;
    renderInfos[i].indexBuffer = bufferID + i + 1;
    renderInfos[i].materialId = materialID + i;
    renderInfos[i].firstInstance = offset;
    renderInfos[i].instanceCount = sizePerDegree[i];
    renderInfos[i].bindingID = shaderOffset + i;
    offset += renderInfos[i].instanceCount;
    actualInfos.push_back(renderInfos[i]);
  }
  shader->bindData(infos.data(), infos.size());
  api->pushRender(actualInfos.size(), actualInfos.data());
  return bufferID;
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

  std::array<uint32_t, MAX_DEGREE> sizePerDegree;
  std::fill(begin(sizePerDegree), end(sizePerDegree), 0);
  sizePerDegree[0] = 1;

  cells[0] = {glm::vec3(1, 0, 0)};

  const auto [materialPoolID, shaderOffset] = createShaderPipes(api, shader);
  const auto bufferPoolID =
      createBuffer(api, shader, materialPoolID, shaderOffset, sizePerDegree);

  const auto startResult = start();
  if (startResult != main::Error::NONE) {
    printf("Error in start!");
    return -1;
  }

  return (uint32_t)startResult;
}