#pragma once

#include <glm/glm.hpp>
#include <graphics/GameGraphicsModule.hpp>
#include <graphics/GameShaderModule.hpp>
#include <graphics/vulkan/VulkanModuleDef.hpp>
#include <graphics/vulkan/VulkanShaderModule.hpp>

using namespace tge::main;
using namespace tge::graphics;
using namespace tge;

struct CreateInfo {
    bool doubleSided = false;
    bool wireFrame = false;
};

std::tuple<uint32_t, uint32_t>
createShaderPipes(tge::graphics::VulkanGraphicsModule *api,
                  tge::shader::VulkanShaderModule *shader, const CreateInfo& createInfo);

uint32_t createBuffer(tge::graphics::VulkanGraphicsModule *api,
                      tge::shader::VulkanShaderModule *shader,
                      const uint32_t materialID, const uint32_t shaderOffset, const glm::mat4 &mat,
                      const size_t offset, const CreateInfo& createInfo);
void makeVulkan();
