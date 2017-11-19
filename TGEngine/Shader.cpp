#include "Shader.hpp"

std::vector<VkPipelineShaderStageCreateInfo> shaders;
std::vector<VkVertexInputAttributeDescription> description_attributes;

void createShader(char* shader, VkShaderStageFlagBits flag) {
	auto binarys = nio::getBinarys(shader);
	VkShaderModule shader_module = {};
	VkShaderModuleCreateInfo info = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		binarys.size(),
		reinterpret_cast<const uint32_t*>(binarys.data())
	};
	last_result = vkCreateShaderModule(device, &info, nullptr, &shader_module);
	HANDEL(last_result)
	size_t size = shaders.size();
	shaders.resize(size + 1);
	shaders[size] = {
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		nullptr,
		0,
		flag,
		shader_module,
		"main",
		nullptr
	};
}

void createShaderInput(uint32_t location, uint32_t offset, VkFormat format) {
	size_t size = description_attributes.size();
	description_attributes.resize(size + 1);
	description_attributes[size] = {
		location,
		0,
		format,
		offset
	};
}

void destroyShaders() {
	for (VkPipelineShaderStageCreateInfo stage_info : shaders) {
		vkDestroyShaderModule(device, stage_info.module, nullptr);
	}
}