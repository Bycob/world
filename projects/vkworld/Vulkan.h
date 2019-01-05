#ifndef WORLDAPI_VULKAN_H
#define WORLDAPI_VULKAN_H

#include "VkWorldConfig.h"

namespace world {

class VulkanContextPrivate;

class VKWORLD_EXPORT VulkanContext {
public:
    VulkanContext();

    ~VulkanContext();

    void configTest();

    void displayAvailableExtensions();

    VulkanContextPrivate &internal();

private:
    VulkanContextPrivate *_internal;
};

struct VKWORLD_EXPORT Vulkan {
    static VulkanContext &context();
};

} // namespace world

/* REFACTOR
 *
 * - Changer le nom des classes : DescriptorType, MemoryType
 * - Passer le VulkanContext en paramètres partout ou nulle part
 * - Destroy resources
 * - register commands on worker a bit less ugly
 */

#endif // WORLDAPI_VULKAN_H
