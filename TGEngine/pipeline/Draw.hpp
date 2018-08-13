#pragma once

#include "..\stdbase.hpp"
#include "Device.hpp"
#include "Window\Window.hpp"
#include "Swapchain.hpp"
#include "CommandBuffer.hpp"
#include "Buffer\IndexBuffer.hpp"

extern std::vector<VkPipelineStageFlags> stage_flags;
extern uint32_t image_index;
extern VkFence fence;
extern UniformBuffer uniform_scale_buffer;

SINCE(0, 0, 3)
void startdraw();

SINCE(0, 0, 3)
void present();

SINCE(0, 0, 1)
void submit();

SINCE(0, 0, 1)
void createSemaphores();

SINCE(0, 0, 1)
void destroySemaphores();