#include "VkwSubBuffer.h"

#include "Vulkan.h"


namespace world {

class VkwSubBufferPrivate {
public:
    IVkwMemoryAccess &_memAccess;
    vk::Buffer _buffer;
    u32 _size;
    u32 _offset;
};

const VkwSubBuffer VkwSubBuffer::NONE = VkwSubBuffer();

VkwSubBuffer::VkwSubBuffer() : _internal(nullptr) {}

VkwSubBuffer::VkwSubBuffer(IVkwMemoryAccess &memAccess, vk::Buffer buffer,
                           u32 size, u32 offset)
        : _internal(std::make_shared<VkwSubBufferPrivate>(
              VkwSubBufferPrivate{memAccess, buffer, size, offset})) {}

u32 VkwSubBuffer::getSize() const { return _internal->_size; }

u32 VkwSubBuffer::getOffset() const { return _internal->_offset; }

vk::Buffer VkwSubBuffer::handle() const { return _internal->_buffer; }

void VkwSubBuffer::getData(void *data) { getData(data, _internal->_size); }

void VkwSubBuffer::getData(void *data, u32 count, u32 offset) {
    _internal->_memAccess.getData(data, count, _internal->_offset + offset);
}

void VkwSubBuffer::setData(const void *data) {
    setData(data, _internal->_size);
}

void VkwSubBuffer::setData(const void *data, u32 count, u32 offset) {
    _internal->_memAccess.setData(data, count, _internal->_offset + offset);
}

void VkwSubBuffer::registerTo(vk::DescriptorSet &descriptorSet,
                              vk::DescriptorType descriptorType, u32 id) {
    vk::DescriptorBufferInfo descriptorBufferInfo(
        _internal->_buffer, _internal->_offset, _internal->_size);
    vk::WriteDescriptorSet writeDescriptorSet(descriptorSet, id, 0, 1,
                                              descriptorType, nullptr,
                                              &descriptorBufferInfo);

    auto &ctx = Vulkan::context();
    ctx._device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

} // namespace world