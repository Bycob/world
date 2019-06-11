#ifndef WORLDAPI_VKMEMORYCACHE_H
#define WORLDAPI_VKMEMORYCACHE_H

#include "VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "VkwEnums.h"
#include "IVkwMemoryAccess.h"
#include "VkwSubBuffer.h"

namespace world {

class VkMemoryCachePrivate;

class VKWORLD_EXPORT VkwMemoryCache : public IVkwMemoryAccess {
public:
    VkwMemoryCache(u32 segmentSize, DescriptorType usage, MemoryType memType);
    ~VkwMemoryCache() override;

    /** Creates a buffer of given size and binds it with one of the memory
     * segments. */
    VkwSubBuffer allocateBuffer(u32 size);

    /** Upload memory to vulkan. This method should always be called before
     * using the memory on the GPU. */
    void flush();

    // ===== IVkwMemoryAccess

    void setData(void *data, u32 count, u32 offset) override;

    void getData(void *data, u32 count, u32 offset) override;

    vk::Buffer getBufferHandle(u32 offset) override;

    u32 getBufferOffset(u32 offset) override;

private:
    std::shared_ptr<VkMemoryCachePrivate> _internal;


    u32 sizeRemaining() const;
};

} // namespace world

#endif