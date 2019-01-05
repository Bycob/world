#ifndef VKWORLD_IVKMEMORYACCESS_H
#define VKWORLD_IVKMEMORYACCESS_H

#include "VkWorldConfig.h"

#include <world/core/WorldTypes.h>

namespace vk {
class Buffer;
}

namespace world {

/** Any class which gives access to a memory space on the GPU. */
class VKWORLD_EXPORT IVkMemoryAccess {
public:
    virtual ~IVkMemoryAccess() = default;

    virtual void setData(void *data, u32 count, u32 offset) = 0;

    virtual void getData(void *data, u32 count, u32 offset) = 0;

    virtual vk::Buffer getBufferHandle(u32 offset) = 0;
};

}; // namespace world

#endif // VKWORLD_IVKMEMORYACCESS_H