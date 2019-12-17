#ifndef VKWORLD_IVKBINDINABLE_H
#define VKWORLD_IVKBINDINABLE_H

#include "vkworld/VkWorldConfig.h"

#include <vulkan/vulkan.hpp>

namespace world {

/** Any class which gives access to a memory space on the GPU. */
class VKWORLD_EXPORT IVkwBindable {
public:
    virtual ~IVkwBindable() = default;

    // TODO this method is ugly
    virtual void registerTo(vk::DescriptorSet &descriptorSet,
                            vk::DescriptorType descriptorType, u32 id) = 0;
};

}; // namespace world

#endif // VKWORLD_IVKBINDINABLE_H
