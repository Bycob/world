#ifndef VKWORLD_VKSUBBUFFER_H
#define VKWORLD_VKSUBBUFFER_H

#include "vkworld/VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "IVkwMemoryAccess.h"
#include "IVkwBindable.h"

namespace world {

class VkwSubBufferPrivate;

class VKWORLD_EXPORT VkwSubBuffer : public IVkwBindable,
                                    public IVkwMemoryAccess {
public:
    static const VkwSubBuffer NONE;

    VkwSubBuffer();
    VkwSubBuffer(IVkwMemoryAccess &memAccess, vk::Buffer buffer, u32 size,
                 u32 offset);

    u32 getSize() const;
    u32 getOffset() const;

    /** Adds this buffer as binding to the given descriptorSet. */
    void registerTo(vk::DescriptorSet &descriptorSet,
                    vk::DescriptorType descriptorType, u32 id) override;

    void getData(void *data);
    void getData(void *data, u32 count, u32 offset = 0) override;

    void setData(void *data);
    void setData(void *data, u32 count, u32 offset = 0) override;

private:
    std::shared_ptr<VkwSubBufferPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_VKSUBBUFFER_H