#include "VkSubBuffer.h"

#include "Vulkan_p.h"

namespace world {

class VkSubBufferPrivate {
public:
    IVkMemoryAccess &_memAccess;

    u32 _size;
    u32 _offset;
};

VkSubBuffer::VkSubBuffer(IVkMemoryAccess &memAccess, u32 size, u32 offset)
        : _internal(std::make_shared<VkSubBufferPrivate>(
              VkSubBufferPrivate{memAccess, size, offset})) {}

VkSubBuffer::VkSubBuffer(const VkSubBuffer &other) = default;

VkSubBuffer &VkSubBuffer::operator=(const VkSubBuffer &other) = default;

u32 VkSubBuffer::getSize() const { return _internal->_size; }

u32 VkSubBuffer::getOffset() const { return _internal->_offset; }


void VkSubBuffer::getData(void *data) { getData(data, _internal->_size); }

void VkSubBuffer::getData(void *data, u32 count, u32 offset) {
    _internal->_memAccess.getData(data, count, _internal->_offset + offset);
}

void VkSubBuffer::setData(void *data) { setData(data, _internal->_size); }

void VkSubBuffer::setData(void *data, u32 count, u32 offset) {
    _internal->_memAccess.setData(data, count, _internal->_offset + offset);
}

void VkSubBuffer::registerTo(vk::DescriptorSet &descriptorSet,
                             vk::DescriptorType descriptorType, u32 id) {
    vk::DescriptorBufferInfo descriptorBufferInfo(
        _internal->_memAccess.getBufferHandle(_internal->_offset),
        _internal->_offset, _internal->_size);
    vk::WriteDescriptorSet writeDescriptorSet(descriptorSet, id, 0, 1,
                                              descriptorType, nullptr,
                                              &descriptorBufferInfo);

    auto &vkctx = Vulkan::context().internal();
    vkctx._device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

} // namespace world