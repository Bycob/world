#ifndef VKWORLD_DESCRIPTORSETLAYOUTVK_H
#define VKWORLD_DESCRIPTORSETLAYOUTVK_H

#include "VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "VkEnums.h"

namespace vk {
class DescriptorSetLayout;
}

namespace world {

class DescriptorSetLayoutVkPrivate;

class VKWORLD_EXPORT DescriptorSetLayoutVk {
public:
    DescriptorSetLayoutVk();

    void addBinding(DescriptorType type, u32 binding);

    /** Get a layout built with the data provided previously. */
    vk::DescriptorSetLayout &getLayout();

private:
    std::shared_ptr<DescriptorSetLayoutVkPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_DESCRIPTORSETLAYOUTVK_H
