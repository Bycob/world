#ifndef VKWORLD_DESCRIPTORSETVK_H
#define VKWORLD_DESCRIPTORSETVK_H

#include "vkworld/VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include <vulkan/vulkan.hpp>

#include "IVkwBindable.h"
#include "VkwEnums.h"
#include "VkwDescriptorSetLayout.h"

namespace world {

class VkwDescriptorSetPrivate;

class VKWORLD_EXPORT VkwDescriptorSet {
public:
    VkwDescriptorSet(VkwDescriptorSetLayout &layout);

    void addDescriptor(u32 id, IVkwBindable &bindable);

    vk::DescriptorSet &handle();

private:
    std::shared_ptr<VkwDescriptorSetPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_DESCRIPTORSETVK_H
