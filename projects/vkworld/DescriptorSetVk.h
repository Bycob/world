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

class DescriptorSetVkPrivate;

class VKWORLD_EXPORT DescriptorSetVk {
public:
    DescriptorSetVk(DescriptorSetLayoutVk &layout);

    void addDescriptor(u32 id, DescriptorType usage, IVkBindable &bindable);

    vk::DescriptorSet &handle();

private:
    std::shared_ptr<DescriptorSetVkPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_DESCRIPTORSETVK_H
