#include "BufferVk.h"

#include <vulkan/vulkan.hpp>

namespace world {

class BufferVkPrivate {
public:
    VulkanContext &_context;

    DescriptorType _descriptorType;

    u32 _memorySize;

    vk::Buffer _buffer;

    vk::DeviceMemory _memory;

    BufferVkPrivate(VulkanContext &context) : _context(context) {}
};

} // namespace world
