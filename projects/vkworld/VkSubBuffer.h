#ifndef VKWORLD_VKSUBBUFFER_H
#define VKWORLD_VKSUBBUFFER_H

#include "VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "IVkMemoryAccess.h"
#include "IVkBindable.h"

namespace world {

class VkwSubBufferPrivate;

class VKWORLD_EXPORT VkwSubBuffer : public IVkwBindable, public IVkwMemoryAccess {
public:
    static const VkwSubBuffer NONE;

    VkwSubBuffer();
    VkwSubBuffer(IVkwMemoryAccess &memAccess, u32 size, u32 offset);

    u32 getSize() const;
    u32 getOffset() const;

    /** Adds this buffer as binding to the given descriptorSet. */
    void registerTo(vk::DescriptorSet &descriptorSet,
                    vk::DescriptorType descriptorType, u32 id) override;

    void getData(void *data);
    void getData(void *data, u32 count, u32 offset = 0) override;

    void setData(void *data);
    void setData(void *data, u32 count, u32 offset = 0) override;


    vk::Buffer getBufferHandle(u32 offset) override;

    /** Get the offset of the buffer containing data at the specified offset. */
    u32 getBufferOffset(u32 offset) override;

private:
    std::shared_ptr<VkwSubBufferPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_VKSUBBUFFER_H