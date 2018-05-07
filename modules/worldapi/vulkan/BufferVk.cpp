#include "BufferVk.h"
#include "PBufferVk.h"

#include "PVulkan.h"

namespace world {

BufferVk::BufferVk(VulkanContext &context, DescriptorType descriptorType, u32 size) 
	: _internal(std::make_shared<PBufferVk>(context)) {

	auto &pcontext = context.internal();

	_internal->_descriptorType = descriptorType;
	_internal->_memorySize = size;

	// Allocate buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;

	switch (descriptorType) {
	case DescriptorType::STORAGE_BUFFER:
		bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		break;
	case DescriptorType::UNIFORM_BUFFER:
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		break;
	}

	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer(pcontext._device, &bufferInfo, nullptr, &_internal->_buffer);

	// Allocate memory;
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(pcontext._device, _internal->_buffer, &memRequirements);

	VkMemoryAllocateInfo memoryInfo = {};
	memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryInfo.pNext = nullptr;
	memoryInfo.allocationSize = memRequirements.size;
	memoryInfo.memoryTypeIndex = pcontext.findMemoryType(size);

	if (vkAllocateMemory(pcontext._device, &memoryInfo, nullptr, &_internal->_memory) != VK_SUCCESS) {
		throw std::runtime_error("[Vulkan] [Config-Test] failed allocate memory");
	}

	// Bind memory
	vkBindBufferMemory(pcontext._device, _internal->_buffer, _internal->_memory, 0);
}

PBufferVk &BufferVk::internal() {
	return *_internal;
}

void BufferVk::setData(void * data, u32 count) {
	auto &internalCtx = _internal->_context.internal();
	
	// TODO if (count > _internal->_memorySize)

	void *mapped;
	vkMapMemory(internalCtx._device, _internal->_memory, 0, _internal->_memorySize, 0, &mapped);
	memcpy(mapped, data, count);
	vkUnmapMemory(internalCtx._device, _internal->_memory);
}

void BufferVk::getData(void * data, u32 count) {
	auto &internalCtx = _internal->_context.internal();

	// TODO if (count > _internal->_memorySize) throw whatever at user's face.

	void *mapped;
	vkMapMemory(internalCtx._device, _internal->_memory, 0, _internal->_memorySize, 0, &mapped);
	memcpy(data, mapped, count);
	vkUnmapMemory(internalCtx._device, _internal->_memory);
}

}