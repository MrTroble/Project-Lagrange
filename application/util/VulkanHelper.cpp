#include "VulkanHelper.hpp"
#define SPR_NO_DEBUG_OUTPUT 1
#define SPR_NO_GLSL_INCLUDE 1
#define SPR_NO_STATIC 1
#define SPR_STATIC extern

#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>

#include <glm/gtx/rotate_vector.hpp>
#include <graphics/vulkan/VulkanShaderPipe.hpp>
#include <limits>
#include <vulkan/vulkan.hpp>
#undef ERROR
#include <TGEngine.hpp>
#include <Util.hpp>
#include <array>
#include <cmath>
#include <headerlibs/ShaderPermute.hpp>
#include <limits>
#include <string>
#include <tuple>
#include <vector>

#include "Calculations.hpp"
#include "DataManager.hpp"
#undef min
#undef max
#include "../TGApp.hpp"

using namespace tge::main;
using namespace tge::graphics;
using namespace tge;

std::tuple<std::vector<TPipelineHolder>, uint32_t> createShaderPipes(
    tge::graphics::VulkanGraphicsModule *api,
    tge::shader::VulkanShaderModule *shader, const CreateInfo &createInfo) {
  permute::glslLookup["min"] = [&](const auto &input) {
    return std::to_string(CellEntry::minMax.x);
  };
  permute::glslLookup["max"] = [&](const auto &input) {
    return std::to_string(CellEntry::minMax.y);
  };
  std::array<Material, MAX_DEGREE * 2> material;
  auto beginShader = shader->pipeInfos.size();

  const std::string vertex = "assets/perInstanceVertexShader.vert";
  const std::string heightFragment = "assets/height.frag";
  const std::string wireFragment = "assets/wire.frag";
  for (size_t i = 0; i < MAX_DEGREE; i++) {
    permute::glslLookup["degree"] = [&](const auto &input) {
      return std::to_string(i);
    };
    permute::glslLookup["steps"] = permute::glslLookup["degree"];
    permute::glslLookup["allpoints"] = [&](const auto &input) {
      return std::to_string(
          std::max(CellEntry::cellDataPerLayer[i].size(), (size_t)1));
    };

    auto shaderpipe =
        shader->loadShaderPipeAndCompile({vertex, heightFragment});
    auto &materialPipe = material[i * 2];
    materialPipe.costumShaderData = shaderpipe;
    materialPipe.primitiveType = (uint32_t)PrimitiveTopology::eTriangleFan;

    const auto generalPipe = shader->loadShaderPipeAndCompile({vertex, wireFragment});
    auto &nextMaterial = material[i*2 + 1];
    nextMaterial.costumShaderData = generalPipe;
    nextMaterial.primitiveType = (uint32_t)PrimitiveTopology::eLineStrip;

    shader->createBindings(shaderpipe, 1);
    shader->createBindings(generalPipe, 1);
  }

  return {api->pushMaterials(material.size(), material.data()), beginShader};
}

std::vector<TDataHolder> createBuffer(
    tge::graphics::VulkanGraphicsModule *api,
    tge::shader::VulkanShaderModule *shader,
    const std::vector<TPipelineHolder> &materials, const uint32_t shaderOffset,
    glm::mat4 &mat, const TRenderHolder offset, const CreateInfo &createInfo) {
  std::vector<BufferInfo> bufferInfos = {
      BufferInfo{&mat, sizeof(mat), DataType::Uniform},
      BufferInfo{&mat, sizeof(mat), DataType::Uniform}};
  std::vector<size_t> layer = {};
  for (size_t i = 0; i < MAX_DEGREE; i++) {
    if (CellEntry::cellDataPerLayer[i].empty()) continue;
    layer.push_back(i);
    bufferInfos.emplace_back(CellEntry::cellDataPerLayer[i].data(),
                             CellEntry::cellDataPerLayer[i].size() *
                                 sizeof(CellEntry::cellDataPerLayer[i][0]),
                             DataType::Uniform);
  }

  const auto bufferHolder = ((APILayer*)api)->pushData(bufferInfos);

  std::vector<RenderInfo> actualInfos;
  std::vector<shader::BindingInfo> infos;
  actualInfos.reserve(MAX_DEGREE);
  infos.reserve(MAX_DEGREE * 2);
  for (size_t i = 0; i < layer.size(); i++) {
    const size_t cLayer = layer[i];

    shader::BindingInfo info;
    info.binding = 0;
    info.bindingSet = shader->pipeInfos[shaderOffset + i * 2].descSet;
    info.type = shader::BindingType::Storage;
    info.data.buffer.dataID = bufferHolder[i + 2];
    info.data.buffer.offset = 0;
    info.data.buffer.size = bufferInfos[i + 2].size;
    infos.push_back(info);

    info.binding = 1;
    info.type = shader::BindingType::UniformBuffer;
    info.data.buffer.dataID = bufferHolder[0];
    info.data.buffer.size = bufferInfos[0].size;
    infos.push_back(info);

    info.binding = 0;
    info.bindingSet = shader->pipeInfos[shaderOffset + i * 2 + 1].descSet;
    info.type = shader::BindingType::Storage;
    info.data.buffer.dataID = bufferHolder[i + 2];
    info.data.buffer.offset = 0;
    info.data.buffer.size = bufferInfos[i + 2].size;
    infos.push_back(info);

    info.binding = 1;
    info.type = shader::BindingType::UniformBuffer;
    info.data.buffer.dataID = bufferHolder[1];
    info.data.buffer.size = bufferInfos[1].size;
    infos.push_back(info);

    RenderInfo renderInfo;
    renderInfo.indexSize = IndexSize::NONE;
    renderInfo.materialId = materials[cLayer * 2];
    renderInfo.firstInstance = 0;
    renderInfo.indexCount = 4;
    renderInfo.instanceCount = CellEntry::cellDataPerLayer[cLayer].size() / 4;
    renderInfo.bindingID = shaderOffset + i * 2;
    actualInfos.push_back(renderInfo);

    if (createInfo.wireFrame) {
      renderInfo.materialId = materials[cLayer * 2 + 1];
      renderInfo.bindingID = shaderOffset + i * 2 + 1;
      actualInfos.push_back(renderInfo);
    }
  }
  shader->bindData(infos.data(), infos.size());
  if (actualInfos.size() > 0) {
    api->pushRender(actualInfos.size(), actualInfos.data(), offset);
  } else {
    //((APILayer *)api)->removeRender(std::array{offset});
    // this is not good
    api->secondaryCommandBuffer.clear();
  }
  return bufferHolder;
}

void makeVulkan() {
  auto api = (tge::graphics::VulkanGraphicsModule *)getAPILayer();
  auto shader = (tge::shader::VulkanShaderModule *)api->getShaderAPI();

  const auto view = api->viewport;
  ioModul->aspectRatio = view.width / view.height;
  const auto vec = makeData(guiModul->currentY, guiModul->interpolation);
  ioModul->implTrans = vec;

  if (CellEntry::updatePipelines) {
    api->device.waitIdle();
    api->removeData(ioModul->bindings, true);

    CreateInfo createInfo;
    createInfo.doubleSided = guiModul->doubleSided;
    createInfo.wireFrame = guiModul->wireFrame;
    const auto &[materialPoolID, shaderOffset] =
        createShaderPipes(api, shader, createInfo);
    const auto bufferPoolID =
        createBuffer(api, shader, materialPoolID, shaderOffset,
                     ioModul->mvpMatrix, 1, createInfo);
    ioModul->bindings = bufferPoolID;
    ioModul->sendChanges();
    api->pushLights(1, &guiModul->light);
    CellEntry::updatePipelines = false;
  } else {
    size_t counter = 1;
    for (size_t i = 0; i < MAX_DEGREE; i++) {
      auto &cellLayer = CellEntry::cellDataPerLayer[i];
      if (cellLayer.empty()) continue;
      counter++;
      std::array bufferChanges{
          BufferChange{ioModul->bindings[counter], cellLayer.data(),
                                cellLayer.size() * sizeof(cellLayer[0])}};
      ((APILayer *)api)->changeData(bufferChanges);
    }
  }
}
