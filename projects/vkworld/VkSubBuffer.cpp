#include "VkSubBuffer.h"

#include "Vulkan_p.h"


namespace world {

class VkwSubBufferPrivate {
public:
    IVkwMemoryAccess &_memAccess;

    u32 _size;
    u32 _offset;
};

const VkwSubBuffer VkwSubBuffer::NONE = VkwSubBuffer();

VkwSubBuffer::VkwSubBuffer() : _internal(nullptr) {}

VkwSubBuffer::VkwSubBuffer(IVkwMemoryAccess &memAccess, u32 size, u32 offset)
        : _internal(std::make_shared<VkwSubBufferPrivate>(
              VkwSubBufferPrivate{memAccess, size, offset})) {}

u32 VkwSubBuffer::getSize() const { return _internal->_size; }

u32 VkwSubBuffer::getOffset() const { return _internal->_offset; }


void VkwSubBuffer::getData(void *data) { getData(data, _internal->_size); }

void VkwSubBuffer::getData(void *data, u32 count, u32 offset) {
    _internal->_memAccess.getData(data, count, _internal->_offset + offset);
}

void VkwSubBuffer::setData(void *data) { setData(data, _internal->_size); }

void VkwSubBuffer::setData(void *data, u32 count, u32 offset) {
    _internal->_memAccess.setData(data, count, _internal->_offset + offset);
}

void VkwSubBuffer::registerTo(vk::DescriptorSet &descriptorSet,
                              vk::DescriptorType descriptorType, u32 id) {
    vk::DescriptorBufferInfo descriptorBufferInfo(
        _internal->_memAccess.getBufferHandle(_internal->_offset),
        _internal->_offset -
            _internal->_memAccess.getBufferOffset(_internal->_offset),
        _internal->_size);
    vk::WriteDescriptorSet writeDescriptorSet(descriptorSet, id, 0, 1,
                                              descriptorType, nullptr,
                                              &descriptorBufferInfo);

    auto &vkctx = Vulkan::context().internal();
    vkctx._device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

vk::Buffer VkwSubBuffer::getBufferHandle(u32 offset) {
    return _internal->_memAccess.getBufferHandle(_internal->_offset);
}

u32 VkwSubBuffer::getBufferOffset(u32 offset) {
    return _internal->_memAccess.getBufferOffset(_internal->_offset);
}

} // namespace world