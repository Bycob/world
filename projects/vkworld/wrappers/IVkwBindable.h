#ifndef VKWORLD_IVKBINDINABLE_H
#define VKWORLD_IVKBINDINABLE_H

#include "VkWorldConfig.h"

#include <vulkan/vulkan.hpp>

namespace world {

/** Any class which gives access to a memory space on the GPU. */
class VKWORLD_EXPORT IVkwBindable {
public:
    virtual ~IVkwBindable() = default;

    virtual void registerTo(vk::DescriptorSet &descriptorSet,
                            vk::DescriptorType descriptorType, u32 id) = 0;
};

}; // namespace world

#endif // VKWORLD_IVKBINDINABLE_H
