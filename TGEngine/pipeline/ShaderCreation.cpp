#include "ShaderCreation.hpp"
#include "Pipe.hpp"

VkPipelineShaderStageCreateInfo createShader(const unsigned char data[], VkShaderStageFlagBits flag, uint32_t size) {
	VkShaderModule shader_module;
	VkShaderModuleCreateInfo info = {
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0,
		size,
		reinterpret_cast<const uint32_t*>(data)
	};
	lastResult = vkCreateShaderModule(device, &info, nullptr, &shader_module);
	HANDEL(lastResult)
		return {
				VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				nullptr,
				0,
				flag,
				shader_module,
				"main",
				nullptr
	};
}

ShaderPipe::ShaderPipe(VkPipelineShaderStageCreateInfo* shader,
	const VkVertexInputAttributeDescription* inputs,
	const VkDescriptorSetLayoutBinding* layouts,
	const unsigned int shaderCount,
	const unsigned int inputCount,
	const unsigned int layoutCount) :
	shaderCount(shaderCount),
	inputCount(inputCount),
	layoutCount(layoutCount),
	shader(shader),
	inputs(const_cast<VkVertexInputAttributeDescription*>(inputs)),
	layouts(const_cast<VkDescriptorSetLayoutBinding*>(layouts)) {

	vlibDescriptorSetLayoutCreateInfo.bindingCount = layoutCount;
	vlibDescriptorSetLayoutCreateInfo.pBindings = layouts;
	this->layoutIndex = createLayouts();
}

void ShaderPipe::precreation() {
	vlibVertexInputState.pVertexAttributeDescriptions = this->inputs;
	vlibVertexInputState.vertexAttributeDescriptionCount = this->inputCount;
	vlibGraphicsPipelineCreateInfo.pStages = this->shader;
	vlibGraphicsPipelineCreateInfo.stageCount = this->shaderCount;
}
