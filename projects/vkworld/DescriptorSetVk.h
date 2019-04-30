#ifndef VKWORLD_DESCRIPTORSETVK_H
#define VKWORLD_DESCRIPTORSETVK_H

#include "VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "IVkBindable.h"
#include "VkEnums.h"
#include "DescriptorSetLayoutVk.h"

namespace vk {
class DescriptorSet;
}

namespace world {

class VkwDescriptorSetPrivate;

class VKWORLD_EXPORT VkwDescriptorSet {
public:
    VkwDescriptorSet(VkwDescriptorSetLayout &layout);

    void addDescriptor(u32 id, DescriptorType usage, IVkwBindable &bindable);

    vk::DescriptorSet &handle();

private:
    std::shared_ptr<VkwDescriptorSetPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_DESCRIPTORSETVK_H
