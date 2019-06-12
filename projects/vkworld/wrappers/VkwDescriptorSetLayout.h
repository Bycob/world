#ifndef VKWORLD_DESCRIPTORSETLAYOUTVK_H
#define VKWORLD_DESCRIPTORSETLAYOUTVK_H

#include "VkWorldConfig.h"

#include <memory>
#include <initializer_list>

#include <vulkan/vulkan.hpp>

#include <world/core/WorldTypes.h>

#include "VkwEnums.h"

namespace world {

class VkwDescriptorSetLayoutPrivate;

class VKWORLD_EXPORT VkwDescriptorSetLayout {
public:
    VkwDescriptorSetLayout();
    VkwDescriptorSetLayout(std::initializer_list<u32> uniformIds,
                           std::initializer_list<u32> storageIds);

    void addBinding(DescriptorType type, u32 binding);

    vk::DescriptorType getBindingType(u32 binding) const;

    /** Get a layout built with the data provided previously. */
    vk::DescriptorSetLayout &getLayout();

private:
    std::shared_ptr<VkwDescriptorSetLayoutPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_DESCRIPTORSETLAYOUTVK_H
