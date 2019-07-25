#ifndef WORLDAPI_BUFFERVK_H
#define WORLDAPI_BUFFERVK_H

#include "VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "wrappers/VkwEnums.h"
#include "wrappers/Vulkan.h"

namespace world {

class BufferVkPrivate;

class VKWORLD_EXPORT BufferVk {
public:
    BufferVk(VulkanContext &context, DescriptorType descriptorType, u32 size);

    /** Creates a new BufferVk that points to the same buffer as this one.
     * Modifying the copied buffer will affect this one. */
    BufferVk(const BufferVk &other);

    BufferVk(BufferVk &&other);

    BufferVk &operator=(const BufferVk &other);

    BufferVk &operator=(BufferVk &&other);

    /** Copy "count" bytes from "data" to the buffer. */
    void setData(void *data, u32 count);

    /** Copy "count" bytes from the buffer to the given data array. */
    void getData(void *data, u32 count);

    BufferVkPrivate &internal();

private:
    std::shared_ptr<BufferVkPrivate> _internal;
};

} // namespace world

#endif // WORLDAPI_BUFFERVK_H