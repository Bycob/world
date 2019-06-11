#include "BufferVk.h"
#include "BufferVk_p.h"

namespace world {

BufferVk::BufferVk(VulkanContext &context, DescriptorType descriptorType,
                   u32 size)
        : _internal(std::make_shared<BufferVkPrivate>(context)) {

    auto &pcontext = context;

    _internal->_descriptorType = descriptorType;
    _internal->_memorySize = size;

    // Allocate buffer
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = size;

    switch (descriptorType) {
    case DescriptorType::STORAGE_BUFFER:
        bufferInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer;
        break;
    case DescriptorType::UNIFORM_BUFFER:
        bufferInfo.usage = vk::BufferUsageFlagBits::eUniformBuffer;
        break;
    }

    // TODO handle concurrent shared buffer
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    pcontext._device.createBuffer(&bufferInfo, nullptr, &_internal->_buffer);

    // Allocate memory;
    vk::MemoryRequirements memRequirements;
    pcontext._device.getBufferMemoryRequirements(_internal->_buffer,
                                                 &memRequirements);

    vk::MemoryAllocateInfo memoryInfo = {};
    memoryInfo.pNext = nullptr;
    memoryInfo.allocationSize = memRequirements.size;
    memoryInfo.memoryTypeIndex = pcontext.findMemoryType(
        size, vk::MemoryPropertyFlagBits::eHostVisible |
                  vk::MemoryPropertyFlagBits::eHostCached);

    _internal->_memory = pcontext._device.allocateMemory(memoryInfo, nullptr);

    // Bind memory
    pcontext._device.bindBufferMemory(_internal->_buffer, _internal->_memory,
                                      0);
}

BufferVk::BufferVk(const BufferVk &other) = default;

BufferVk::BufferVk(BufferVk &&other) = default; // TODO check

BufferVk &BufferVk::operator=(const BufferVk &other) = default;

BufferVk &BufferVk::operator=(BufferVk &&other) = default;

BufferVkPrivate &BufferVk::internal() { return *_internal; }

void BufferVk::setData(void *data, u32 count) {
    auto &internalCtx = _internal->_context;

    // TODO if (count > _internal->_memorySize)

    void *mapped = internalCtx._device.mapMemory(_internal->_memory, 0,
                                                 _internal->_memorySize);
    memcpy(mapped, data, count);
    internalCtx._device.unmapMemory(_internal->_memory);
}

void BufferVk::getData(void *data, u32 count) {
    auto &internalCtx = _internal->_context;

    // TODO if (count > _internal->_memorySize) throw whatever at user's face.

    void *mapped;
    vkMapMemory(internalCtx._device, _internal->_memory, 0,
                _internal->_memorySize, 0, &mapped);
    memcpy(data, mapped, count);
    vkUnmapMemory(internalCtx._device, _internal->_memory);
}

} // namespace world