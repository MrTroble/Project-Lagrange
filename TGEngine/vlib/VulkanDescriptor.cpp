#include "VulkanDescriptor.hpp"

VkDescriptorSetLayoutCreateInfo vlib_descriptor_set_layout_create_info = {
	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
	nullptr,
	0,
	0,
	nullptr
};

VkDescriptorSetAllocateInfo vlib_allocate_info = {
	VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
	nullptr,
	nullptr,
	1,
	VK_NULL_HANDLE
};

VkDescriptorPoolCreateInfo vlib_descriptor_pool_create_info = {
	VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
	nullptr,
	VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
	10,
	0,
	nullptr
};

VkDescriptorPoolSize vlib_descriptor_pool_size = {
	VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
	0
};

VkWriteDescriptorSet vlib_descriptor_writes = {
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		nullptr,
		0,
		0,
		1,
		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		nullptr,
		VK_NULL_HANDLE,
		nullptr
};