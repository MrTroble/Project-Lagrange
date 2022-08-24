#include "VulkanHelper.hpp"
#define SPR_NO_DEBUG_OUTPUT 1
#define SPR_NO_GLSL_INCLUDE 1
#define SPR_NO_STATIC 1
#define SPR_STATIC extern

#include <limits>
#include <glm/gtx/rotate_vector.hpp>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <graphics/vulkan/VulkanShaderPipe.hpp>
#include <vulkan/vulkan.hpp>
#undef ERROR
#include "Calculations.hpp"
#include "DataManager.hpp"
#include <TGEngine.hpp>
#include <Util.hpp>
#include <array>
#include <cmath>
#include <headerlibs/ShaderPermute.hpp>
#include <limits>
#include <string>
#include <tuple>
#include <vector>
#undef min
#undef max
#include "../TGApp.hpp"

using namespace tge::main;
using namespace tge::graphics;
using namespace tge;

namespace permute
{
  std::map<std::string, int> lookupCounter;
  permute::lookup glslLookup = {{"next", next}};
} // namespace permute

std::tuple<uint32_t, uint32_t>
createShaderPipes(tge::graphics::VulkanGraphicsModule *api,
                  tge::shader::VulkanShaderModule *shader, const CreateInfo &createInfo)
{
  auto fragment = permute::fromFile<permute::PermuteGLSL>("assets/height.frag");
  permute::glslLookup["min"] = [&](const auto &input)
  {
    return std::to_string(CellEntry::minMax.x);
  };
  permute::glslLookup["max"] = [&](const auto &input)
  {
    return std::to_string(CellEntry::minMax.y);
  };
  if (!fragment.generate())
  {
    for (auto &str : fragment.getContent())
      printf("%s\n", str.c_str());
    return {-1, -1};
  }

  auto wireFrag = permute::fromFile<permute::PermuteGLSL>("assets/wire.frag");
  if (!wireFrag.generate())
  {
    for (auto &str : wireFrag.getContent())
      printf("%s\n", str.c_str());
    return {-1, -1};
  }

  auto perm = permute::fromFile<permute::PermuteGLSL>(
      "assets/perInstanceVertexShader.vert");
  std::array<Material, MAX_DEGREE * 2> material;
  auto beginShader = shader->pipeInfos.size();
  for (size_t i = 0; i < MAX_DEGREE; i++)
  {
    permute::glslLookup["degree"] = [&](const auto &input)
    {
      return std::to_string(i);
    };
    permute::glslLookup["steps"] = permute::glslLookup["degree"];
    permute::glslLookup["allpoints"] = [&](const auto &input)
    {
      return std::to_string(
          std::max(CellEntry::cellDataPerLayer[i].size(), (size_t)1));
    };
    if (!perm.generate({std::to_string(i)}))
    {
      for (auto &str : perm.getContent())
        printf("%s\n", str.c_str());
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
    for (const auto &binding : shaderpipe->descriptorLayoutBindings)
    {
      descPoolSizes.push_back(
          {binding.descriptorType, binding.descriptorCount * 2});
    }
    const vk::DescriptorPoolCreateInfo descPoolCreateInfo({}, 2, descPoolSizes);
    const auto descPool = api->device.createDescriptorPool(descPoolCreateInfo);
    shader->descPools.push_back(descPool);

    const auto layoutCreateInfo = vk::PipelineLayoutCreateInfo({}, descLayout);
    const auto pipeLayout = api->device.createPipelineLayout(layoutCreateInfo);
    shader->pipeLayouts.push_back(pipeLayout);
    shaderpipe->layoutID = shader->pipeLayouts.size() - 1;

    auto &currentMat = material[i * 2];
    currentMat.costumShaderData = shaderpipe;
    currentMat.primitiveType = (uint32_t)PrimitiveTopology::eTriangleFan;
    currentMat.doubleSided = createInfo.doubleSided;
    auto &nextMat = material[i * 2 + 1];
    shaderpipe->needsDefaultBindings = false;

    const auto generalPipe = new shader::VulkanShaderPipe(*shaderpipe);
    generalPipe->shader[0] = {wireFrag.getBinary(), vk::ShaderStageFlagBits::eFragment};
    nextMat.costumShaderData = generalPipe;
    nextMat.primitiveType = (uint32_t)PrimitiveTopology::eLineStrip;

    shader->createBindings(shaderpipe, 1);
    shader->createBindings(generalPipe, 1);
  }

  return {api->pushMaterials(material.size(), material.data()), beginShader};
}

uint32_t createBuffer(tge::graphics::VulkanGraphicsModule *api,
                      tge::shader::VulkanShaderModule *shader,
                      const uint32_t materialID, const uint32_t shaderOffset, const glm::mat4 &mat,
                      const size_t offset, const CreateInfo &createInfo)
{
  std::vector<size_t> sizes = {sizeof(mat), sizeof(mat)};
  std::vector<const void *> data = {&mat, &mat};
  std::vector<size_t> layer = {};
  for (size_t i = 0; i < MAX_DEGREE; i++)
  {
    if (CellEntry::cellDataPerLayer[i].empty())
      continue;
    layer.push_back(i);
    sizes.push_back(CellEntry::cellDataPerLayer[i].size() *
                    sizeof(CellEntry::cellDataPerLayer[i][0]));
    data.push_back(CellEntry::cellDataPerLayer[i].data());
  }

  const auto bufferID =
      api->pushData(data.size(), data.data(), sizes.data(), DataType::All);

  std::vector<RenderInfo> actualInfos;
  std::vector<shader::BindingInfo> infos;
  actualInfos.reserve(MAX_DEGREE);
  infos.reserve(MAX_DEGREE * 2);
  for (size_t i = 0; i < layer.size(); i++)
  {
    const size_t cLayer = layer[i];

    shader::BindingInfo info;
    info.binding = 0;
    info.bindingSet = shader->pipeInfos[shaderOffset + i * 2].descSet;
    info.type = shader::BindingType::Storage;
    info.data.buffer.dataID = bufferID + i + 2;
    info.data.buffer.offset = 0;
    info.data.buffer.size = sizes[i + 2];
    infos.push_back(info);

    info.binding = 1;
    info.type = shader::BindingType::UniformBuffer;
    info.data.buffer.dataID = bufferID;
    info.data.buffer.size = sizes[i];
    infos.push_back(info);

    info.binding = 0;
    info.bindingSet = shader->pipeInfos[shaderOffset + i * 2 + 1].descSet;
    info.type = shader::BindingType::Storage;
    info.data.buffer.dataID = bufferID + i + 2;
    info.data.buffer.offset = 0;
    info.data.buffer.size = sizes[i + 2];
    infos.push_back(info);

    info.binding = 1;
    info.type = shader::BindingType::UniformBuffer;
    info.data.buffer.dataID = bufferID + 1;
    info.data.buffer.size = sizes[i + 1];
    infos.push_back(info);

    RenderInfo renderInfo;
    renderInfo.indexSize = IndexSize::NONE;
    renderInfo.materialId = materialID + cLayer * 2;
    renderInfo.firstInstance = 0;
    renderInfo.indexCount = 4;
    renderInfo.instanceCount = CellEntry::cellDataPerLayer[cLayer].size() / 4;
    renderInfo.bindingID = shaderOffset + i * 2;
    actualInfos.push_back(renderInfo);

    if (createInfo.wireFrame)
    {
      renderInfo.materialId = materialID + cLayer * 2 + 1;
      renderInfo.bindingID = shaderOffset + i * 2 + 1;
      actualInfos.push_back(renderInfo);
    }
  }
  shader->bindData(infos.data(), infos.size());
  if (actualInfos.size() > 0)
  {
    api->pushRender(actualInfos.size(), actualInfos.data(), api->secondaryCommandBuffer.empty() ? 0 : offset);
  }
  else
  {
    api->secondaryCommandBuffer.clear();
  }
  return bufferID;
}

void makeVulkan()
{
  auto api = (tge::graphics::VulkanGraphicsModule *)getAPILayer();
  auto shader = (tge::shader::VulkanShaderModule *)api->getShaderAPI();

  const auto view = api->viewport;
  ioModul->aspectRatio = view.width / view.height;
  const auto vec = makeData(guiModul->currentY, guiModul->interpolation);
  ioModul->implTrans = vec;

  if (CellEntry::updatePipelines)
  {
    api->device.waitIdle();
    for (size_t i = ioModul->binding; i < api->bufferList.size(); i++)
    {
      api->device.freeMemory(api->bufferMemoryList[i]);
      api->device.destroyBuffer(api->bufferList[i]);
      api->bufferMemoryList[i] = VK_NULL_HANDLE;
      api->bufferList[i] = VK_NULL_HANDLE;
    }

    CreateInfo createInfo;
    createInfo.doubleSided = guiModul->doubleSided;
    createInfo.wireFrame = guiModul->wireFrame;
    const auto [materialPoolID, shaderOffset] = createShaderPipes(api, shader, createInfo);
    const auto bufferPoolID = createBuffer(api, shader, materialPoolID,
                                           shaderOffset, ioModul->mvpMatrix, 1, createInfo);
    ioModul->binding = bufferPoolID;
    ioModul->sendChanges();
    api->pushLights(1, &guiModul->light);
    CellEntry::updatePipelines = false;
  }
  else
  {
    size_t counter = 1;
    for (size_t i = 0; i < MAX_DEGREE; i++)
    {
      const auto &cellLayer = CellEntry::cellDataPerLayer[i];
      if (cellLayer.empty())
        continue;
      counter++;
      api->changeData(ioModul->binding + counter, cellLayer.data(), cellLayer.size() * sizeof(cellLayer[0]));
    }
  }
}
