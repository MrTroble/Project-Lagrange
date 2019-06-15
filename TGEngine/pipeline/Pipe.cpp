#include "Pipe.hpp"

std::vector<VkPipeline> pipelines; 
std::vector<VkPipelineLayout> layouts;

void createPipeline(const VkPipelineShaderStageCreateInfo createinfos[], uint32_t count, uint32_t layout) {
	Window* win = window_list[0];
	vlib_scissor.extent.height = (uint32_t) (vlib_viewport.height = (float) win->height);
	vlib_scissor.extent.width = (uint32_t) (vlib_viewport.width = (float) win->width);
	
	vlib_multisample_state.rasterizationSamples = used_msaa_flag;

	vlib_graphics_pipeline_create_info.layout = layouts[layout];
	vlib_graphics_pipeline_create_info.renderPass = render_pass;

	TG_VECTOR_GET_SIZE_AND_RESIZE(pipelines)
	lastResult = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &vlib_graphics_pipeline_create_info, nullptr, &pipelines[last_size]);
	HANDEL(lastResult);
}

uint32_t createPipelineLayout(uint32_t layout_count, VkDescriptorSetLayout* descriptor_set_layout) {
	last_size = layouts.size();
	layouts.resize(last_size + 1);

	vlib_layout_info.setLayoutCount = layout_count;
	vlib_layout_info.pSetLayouts = descriptor_set_layout;
	lastResult = vkCreatePipelineLayout(device, &vlib_layout_info, nullptr, &layouts[last_size]);
	HANDEL(lastResult);

	return last_size;
}

void destroyPipelineLayout(uint32_t layout) {
	vkDestroyPipelineLayout(device, layouts[layout], nullptr);
}

void destroyPipeline(uint32_t layout)
{
	vkDestroyPipeline(device, pipelines[layout], nullptr);
}

void destroyPipeline() {
	for each (VkPipelineLayout var in layouts)
	{
		vkDestroyPipelineLayout(device, var, nullptr);
	}
	for each (VkPipeline var in pipelines)
	{
		vkDestroyPipeline(device, var, nullptr);
	}
	layouts.clear();
	pipelines.clear();
}