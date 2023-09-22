#pragma once

#include <glm/glm.hpp>
#include <graphics/GameGraphicsModule.hpp>
#include <graphics/GameShaderModule.hpp>
#include <graphics/vulkan/VulkanModuleDef.hpp>
#include <graphics/vulkan/VulkanShaderModule.hpp>
#include <tuple>
#include <vector>

using namespace tge::main;
using namespace tge::graphics;
using namespace tge;

struct CreateInfo {
  bool doubleSided = false;
  bool wireFrame = false;
};

std::tuple<std::vector<TPipelineHolder>, uint32_t> createShaderPipes(
    tge::graphics::VulkanGraphicsModule *api,
    tge::shader::VulkanShaderModule *shader, const CreateInfo &createInfo);

std::vector<TDataHolder> createBuffer(
    tge::graphics::VulkanGraphicsModule *api,
    tge::shader::VulkanShaderModule *shader,
    const std::vector<TPipelineHolder> &materials, const uint32_t shaderOffset,
    glm::mat4 &mat, const TRenderHolder offset, const CreateInfo &createInfo);

void makeVulkan();
