#ifndef VKWORLD_VKSUBBUFFER_H
#define VKWORLD_VKSUBBUFFER_H

#include "VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "IVkMemoryAccess.h"
#include "IVkBindable.h"

namespace world {

class VkSubBufferPrivate;

class VKWORLD_EXPORT VkSubBuffer : public IVkBindable {
public:
    VkSubBuffer(IVkMemoryAccess &memAccess, u32 size, u32 offset);

    VkSubBuffer(const VkSubBuffer &other);

    VkSubBuffer &operator=(const VkSubBuffer &other);

    u32 getSize() const;
    u32 getOffset() const;

    void registerTo(vk::DescriptorSet &descriptorSet,
                    vk::DescriptorType descriptorType, u32 id) override;

private:
    std::shared_ptr<VkSubBufferPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_VKSUBBUFFER_H