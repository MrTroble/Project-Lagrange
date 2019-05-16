#include "CommandBuffer.hpp"


VkCommandPool command_pool;
VkFence single_time_command_ready;
std::vector<VkCommandBuffer> command_buffers;
VkDeviceSize offsets = 0;
bool started = true;

void createCommandBuffer() {
	command_buffers.resize(image_count + 1);

	if (!command_pool) {
		VkCommandPoolCreateInfo commmand_pool_create_info = {
			VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			nullptr,
			VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			queue_index
		};

		last_result = vkCreateCommandPool(device, &commmand_pool_create_info, nullptr, &command_pool);
		HANDEL(last_result)

		vlib_command_buffer_allocate_info.commandPool = command_pool;
	}

	vlib_command_buffer_allocate_info.commandBufferCount = image_count;
	last_result = vkAllocateCommandBuffers(device, &vlib_command_buffer_allocate_info, command_buffers.data());
	HANDEL(last_result)

	vlib_command_buffer_allocate_info.commandBufferCount = 1;
	last_result = vkAllocateCommandBuffers(device, &vlib_command_buffer_allocate_info, &SINGELTIME_COMMAND_BUFFER);
	HANDEL(last_result)

	VkFenceCreateInfo fence_create_info = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	last_result = vkCreateFence(device, &fence_create_info, nullptr, &single_time_command_ready);
	HANDEL(last_result)
}

void startSingleTimeCommand() {
	last_result = vkResetFences(device, 1, &single_time_command_ready);
	HANDEL(last_result)

	vlib_command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	vlib_command_buffer_begin_info.pInheritanceInfo = nullptr;
	last_result = vkBeginCommandBuffer(SINGELTIME_COMMAND_BUFFER, &vlib_command_buffer_begin_info);
	HANDEL(last_result);
}

void endSingleTimeCommand() {
	last_result = vkEndCommandBuffer(SINGELTIME_COMMAND_BUFFER);
	HANDEL(last_result);

	VkSubmitInfo submitInfo = {
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		0,
		nullptr,
		nullptr,
		1,
		&SINGELTIME_COMMAND_BUFFER,
		0,
		nullptr,
	};

	last_result = vkQueueSubmit(queue, 1, &submitInfo, single_time_command_ready);
	HANDEL(last_result)

	last_result = vkWaitForFences(device, 1, &single_time_command_ready, VK_TRUE, UINT64_MAX);
	HANDEL(last_result)
}

void startupCommands() {
	startSingleTimeCommand();

	for each(Texture* tex in textures) {
		tex->load(SINGELTIME_COMMAND_BUFFER);
	}

	vlib_image_memory_barrier.subresourceRange.levelCount = 1;
	vlib_image_memory_barrier.subresourceRange.baseMipLevel = 0;
	ADD_IMAGE_MEMORY_BARRIER(SINGELTIME_COMMAND_BUFFER, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, color_image, 0, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)

	vlib_image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	ADD_IMAGE_MEMORY_BARRIER(SINGELTIME_COMMAND_BUFFER, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, depth_image, 0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)

	for each(StagingBuffer* buf in staging_buffer)
	{
		vlib_buffer_copy.dstOffset = vlib_buffer_copy.srcOffset = 0;
		vlib_buffer_copy.size = buf->size;
		vkCmdCopyBuffer(
			SINGELTIME_COMMAND_BUFFER,
			buf->staging_buffer,
			*buf->destination,
			1,
			&vlib_buffer_copy
		);
	}

	endSingleTimeCommand();

	for each(Texture* tex in textures) {
		tex->vulkanTexture->dispose();
	}
}

void fillCommandBuffer(IndexBuffer* ibuffer, VertexBuffer* vbuffer) {
	for (size_t i = 0; i < image_count; i++)
	{
		VkCommandBuffer buffer = command_buffers[i];

		VkCommandBufferInheritanceInfo command_buffer_inheritance_info = {
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
				nullptr,
				render_pass,
				0,
				frame_buffer[i],
				VK_FALSE,
				0,
				0
		};

		vlib_command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		vlib_command_buffer_begin_info.pInheritanceInfo = &command_buffer_inheritance_info;
		last_result = vkBeginCommandBuffer(buffer, &vlib_command_buffer_begin_info);
		HANDEL(last_result);

		VkRenderPassBeginInfo render_pass_begin_info = {
			VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			nullptr,
			render_pass,
			frame_buffer[i],
			{
				0,
				0,
				(uint32_t)window_list[0]->width,
				(uint32_t)window_list[0]->height
			},
			2,
			vlib_clear_values
		};
		vkCmdBeginRenderPass(buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindVertexBuffers(buffer, 0, 1, &vbuffer->vertex_buffer, &offsets);

		vkCmdBindIndexBuffer(buffer, ibuffer->index_buffer, 0, VK_INDEX_TYPE_UINT32);

		for each(RenderOffsets coffset in render_offset)
		{
			Material mat = materials[coffset.material];

			vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelines[mat.pipeline_index]);

			vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layouts[mat.layout_index], 0, 1, &descriptor_set[mat.descriptor_index], 0, nullptr);

			vkCmdDrawIndexed(buffer, coffset.size, 1, coffset.offset, 0, 0);
		}

		vkCmdEndRenderPass(buffer);

		last_result = vkEndCommandBuffer(buffer);
		HANDEL(last_result)
	}
}

void destroyCommandBuffer() {
	last_result =  vkDeviceWaitIdle(device);
	HANDEL(last_result)
	vkFreeCommandBuffers(device, command_pool, (uint32_t)command_buffers.size(), command_buffers.data());
	vkDestroyCommandPool(device, command_pool, nullptr);
	vkDestroyFence(device, single_time_command_ready, nullptr);
}