#include "VertexBuffer.hpp"

void createVertexBuffer(VertexBuffer* buffer_storage) {
	VkBuffer vertex_buffer;

	VkBufferCreateInfo vertex_buffer_create_info = {
		VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
	    nullptr,
		0,
		VERTEX_SIZE * buffer_storage->max_vertex_count,
	    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	    VK_SHARING_MODE_EXCLUSIVE,
	    0,
	    nullptr
	};

	last_result = vkCreateBuffer(device, &vertex_buffer_create_info, nullptr, &vertex_buffer);
	HANDEL(last_result)

	buffer_storage->vertex_buffer_index = getMemoryRequirements(vertex_buffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void VertexBuffer::start() {
	mapMemory(this->vertex_buffer_index, &this->memory);
	this->count_of_points = 0;
}

void VertexBuffer::add(TGVertex vert) {
	memcpy((TGVertex*)this->memory + this->count_of_points, &vert, VERTEX_SIZE);
	this->count_of_points++;
}

void VertexBuffer::addColorOnly(TGVertex vert) {
	vert.color_only = VK_TRUE;
	this->add(vert);
}

void VertexBuffer::addTexOnly(TGVertex vert) {
	vert.color_only = VK_FALSE;
	vert.color = { 1, 1, 1, 1 } ;
	this->add(vert);
}

void VertexBuffer::addAll(TGVertex* verts, uint32_t count) {
	for (size_t i = 0; i < count; i++)
	{
		this->add(verts[i]);
	}
}

void VertexBuffer::end() {
	unmapMemory();
}