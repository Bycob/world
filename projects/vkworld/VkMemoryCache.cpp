#include "VkMemoryCache.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan_p.h"

namespace world {

class VkMemoryCachePrivate {
public:
    struct Segment {
        Segment(u32 size, DescriptorType descriptorType, MemoryType memType)
                : _data(new char[size]), _totalSize(size), _sizeAllocated(0),
                  _updated(false) {
            VulkanContextPrivate &ctx = Vulkan::context().internal();

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

            _buffer = ctx._device.createBuffer(bufferInfo, nullptr);

            // create memory
            vk::MemoryRequirements memRequirements =
                ctx._device.getBufferMemoryRequirements(_buffer);

            vk::MemoryPropertyFlags requiredProperties;
            vk::MemoryPropertyFlags unwantedProperties;

            switch (memType) {
            case MemoryType::CPU_READS:
                requiredProperties = vk::MemoryPropertyFlagBits::eHostVisible |
                                     vk::MemoryPropertyFlagBits::eHostCoherent;
                break;
            case MemoryType::CPU_WRITES:
                requiredProperties = vk::MemoryPropertyFlagBits::eHostVisible;
                break;
            case MemoryType::GPU_ONLY:
                requiredProperties = vk::MemoryPropertyFlagBits::eDeviceLocal;
                unwantedProperties = vk::MemoryPropertyFlagBits::eHostVisible;
                break;
            }

            vk::MemoryAllocateInfo memoryInfo = {};
            memoryInfo.pNext = nullptr;
            memoryInfo.allocationSize = memRequirements.size;
            memoryInfo.memoryTypeIndex = ctx.findMemoryType(
                size, requiredProperties, unwantedProperties);

            _memory = ctx._device.allocateMemory(memoryInfo);

            ctx._device.bindBufferMemory(_buffer, _memory, 0);
        }

        std::unique_ptr<char[]> _data;
        vk::DeviceMemory _memory;
        vk::Buffer _buffer;

        u32 _totalSize;
        u32 _sizeAllocated;
        /** Indicates if ... uh I don't know yet */
        bool _updated;
    };

    /** Size of each segment of memory allocated by this memory cache */
    u32 _segmentSize;

    DescriptorType _usage;
    MemoryType _memType;

    std::vector<Segment> _segments;
};

VkMemoryCache::VkMemoryCache(u32 segmentSize, DescriptorType usage,
                             MemoryType memType)
        : _internal(std::make_shared<VkMemoryCachePrivate>()) {

    _internal->_segmentSize = segmentSize;
    _internal->_usage = usage;
    _internal->_memType = memType;
}

VkMemoryCache::~VkMemoryCache() = default;

VkSubBuffer VkMemoryCache::allocateBuffer(u32 size) {
    const u32 segmentSize = _internal->_segmentSize;

    if (size > segmentSize) {
        throw std::invalid_argument("Requested size " + std::to_string(size) +
                                    " > Segment size " +
                                    std::to_string(segmentSize));
    }

    if (sizeRemaining() < size) {
        _internal->_segments.emplace_back(segmentSize, _internal->_usage,
                                          _internal->_memType);
    }

    auto &lastSegment = _internal->_segments.back();
    const u32 offset =
        static_cast<u32>(_internal->_segments.size() - 1) * segmentSize +
        lastSegment._sizeAllocated;
    return VkSubBuffer{*this, size, offset};
}

void VkMemoryCache::flush() {
    // Nothing yet
}

void VkMemoryCache::setData(void *data, u32 count, u32 offset) {
    const u32 segmentSize = _internal->_segmentSize;
    const u32 inSegmentOffset = offset % segmentSize;
    const u32 segmentId = offset / segmentSize;
    auto &segment = _internal->_segments[segmentId];

    VulkanContextPrivate &ctx = Vulkan::context().internal();

    void *mapped =
        ctx._device.mapMemory(segment._memory, inSegmentOffset, count);
    memcpy(mapped, data, count);
    ctx._device.unmapMemory(segment._memory);
}

void VkMemoryCache::getData(void *data, u32 count, u32 offset) {
    const u32 segmentSize = _internal->_segmentSize;
    const u32 inSegmentOffset = offset % segmentSize;
    const u32 segmentId = offset / segmentSize;
    auto &segment = _internal->_segments[segmentId];

    VulkanContextPrivate &ctx = Vulkan::context().internal();

    void *mapped =
        ctx._device.mapMemory(segment._memory, inSegmentOffset, count);
    memcpy(data, mapped, count);
    ctx._device.unmapMemory(segment._memory);
}

vk::Buffer VkMemoryCache::getBufferHandle(u32 offset) {
    const u32 segmentSize = _internal->_segmentSize;
    const u32 segmentId = offset / segmentSize;
    auto &segment = _internal->_segments[segmentId];

    return segment._buffer;
}

u32 VkMemoryCache::sizeRemaining() const {
    if (_internal->_segments.size() == 0) {
        return 0;
    }

    auto &lastSegment = _internal->_segments.back();
    return lastSegment._totalSize - lastSegment._sizeAllocated;
}
} // namespace world