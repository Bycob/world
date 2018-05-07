#include "BufferVk.h"

#include <vulkan.h>

namespace world {
	
class PBufferVk {
public:
	VulkanContext &_context;

	DescriptorType _descriptorType;

	u32 _memorySize;

	VkBuffer _buffer;

	VkDeviceMemory _memory;

	PBufferVk(VulkanContext &context) : _context(context) {}
};

}
