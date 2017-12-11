#include "Memory.hpp"

std::vector<VkBuffer*> buffers = {};
std::vector<VkDeviceSize> buffer_sizes = {};
std::vector<VkDeviceSize> buffer_offsets = { 0 };
VkDeviceMemory device_memory;

uint32_t index;
VkDeviceSize _impl_size;
uint32_t memory_flags;

uint32_t getMemoryRequirements(VkBuffer* buffer, uint32_t memoryflags) {
	memory_flags |= memoryflags;

	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(device, *buffer, &requirements);

	_impl_size += requirements.size;

	uint32_t csize = buffer_offsets.size();
	buffer_offsets.resize(csize + 1);
	buffer_offsets[csize] = buffer_offsets[csize - 1] + requirements.size;

	csize = buffer_sizes.size();
	buffer_sizes.resize(csize + 1);
	buffer_sizes[csize] = requirements.size;

	csize = buffers.size();
	buffers.resize(csize + 1);
	buffers[csize] = buffer;

	return csize;
}
 
void allocateAllBuffers() {
	for (index = 0; index < memory_properties.memoryTypeCount; index++) {
		if (memory_properties.memoryTypes[index].propertyFlags & (memory_flags)) break;
	}

	VkMemoryAllocateInfo memory_allocation_info = {
		VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		nullptr,
		_impl_size,
		index
	};

	last_result = vkAllocateMemory(device, &memory_allocation_info, nullptr, &device_memory);
	HANDEL(last_result)
	
	for (int i = 0; i < buffers.size();i++) {
		last_result = vkBindBufferMemory(device, *buffers[i], device_memory, buffer_offsets[i]);
		HANDEL(last_result)
	}
}

void mapMemory(uint32_t buffer_index, void** mapped_memory) {
	last_result = vkMapMemory(device, device_memory, buffer_offsets[buffer_index], buffer_sizes[buffer_index], 0, mapped_memory);
	HANDEL(last_result)
}

void unmapMemory() {
	vkUnmapMemory(device, device_memory);
}

void destroyMemory() {
	vkFreeMemory(device, device_memory, nullptr);
}