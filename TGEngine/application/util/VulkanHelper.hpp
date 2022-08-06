#pragma once
#define SPR_NO_GLSL_INCLUDE 1
#define SPR_NO_STATIC 1
#define SPR_STATIC extern

#include <glm/glm.hpp>
#include <graphics/GameGraphicsModule.hpp>
#include <graphics/GameShaderModule.hpp>
#include <graphics/vulkan/VulkanModuleDef.hpp>
#include <graphics/vulkan/VulkanShaderModule.hpp>

using namespace tge::main;
using namespace tge::graphics;
using namespace tge;

std::tuple<uint32_t, uint32_t>
createShaderPipes(tge::graphics::VulkanGraphicsModule *api,
                  tge::shader::VulkanShaderModule *shader);

uint32_t createBuffer(tge::graphics::VulkanGraphicsModule *api,
                      tge::shader::VulkanShaderModule *shader,
                      uint32_t materialID, uint32_t shaderOffset, glm::mat4 mat,
                      const size_t offset = 0);
void makeVulkan();
