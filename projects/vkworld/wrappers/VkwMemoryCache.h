#ifndef WORLDAPI_VKMEMORYCACHE_H
#define WORLDAPI_VKMEMORYCACHE_H

#include "vkworld/VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "VkwEnums.h"
#include "IVkwMemoryAccess.h"
#include "VkwSubBuffer.h"
#include "VkwImage.h"

namespace world {

struct VKWORLD_EXPORT VkwMemoryCacheSegment : public IVkwMemoryAccess {
    std::unique_ptr<char[]> _data;
    vk::DeviceMemory _memory;
    vk::Buffer _buffer;

    u32 _totalSize;
    u32 _sizeAllocated;
    u32 _alignment;
    /** Indicates if ... uh I don't know yet */
    bool _updated;


    VkwMemoryCacheSegment(u32 size, DescriptorType descriptorType,
                          u32 memTypeIndex);

    ~VkwMemoryCacheSegment() override;

    // ===== IVkwMemoryAccess

    void setData(void *data, u32 count, u32 offset) override;

    void getData(void *data, u32 count, u32 offset) override;
};

class VKWORLD_EXPORT VkwMemoryCache {
public:
    VkwMemoryCache(u32 segmentSize, DescriptorType usage, MemoryType memType);

    VkwMemoryCache(const VkwMemoryCache &other) = delete;

    VkwMemoryCache &operator=(VkwMemoryCache &other) = delete;

    /** Creates a buffer of given size and binds it with one of the memory
     * segments. */
    VkwSubBuffer allocateBuffer(u32 size);

    void allocateImage(VkwImage &image);

    /** Upload memory to vulkan. This method should always be called before
     * using the memory on the GPU. */
    void flush();

private:
    /** Size of each segment of memory allocated by this memory cache */
    u32 _segmentSize;

    DescriptorType _usage;
    MemoryType _memType;

    u32 _memTypeIndex;

    std::vector<std::unique_ptr<VkwMemoryCacheSegment>> _segments;

    u32 sizeRemaining() const;

    // returns offset of allocated memory
    u32 allocate(u32 size);
};

} // namespace world

#endif