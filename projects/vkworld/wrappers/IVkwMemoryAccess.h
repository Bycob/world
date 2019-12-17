#ifndef VKWORLD_IVKMEMORYACCESS_H
#define VKWORLD_IVKMEMORYACCESS_H

#include "vkworld/VkWorldConfig.h"

#include <vulkan/vulkan.hpp>

#include <world/core/WorldTypes.h>

namespace world {

/** Any class which gives access to a memory space on the GPU. */
class VKWORLD_EXPORT IVkwMemoryAccess {
public:
    virtual ~IVkwMemoryAccess() = default;

    virtual void setData(const void *data, u32 count, u32 offset) = 0;

    virtual void getData(void *data, u32 count, u32 offset) = 0;
};

}; // namespace world

#endif // VKWORLD_IVKMEMORYACCESS_H