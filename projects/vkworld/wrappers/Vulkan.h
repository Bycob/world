#ifndef WORLDAPI_VULKAN_H
#define WORLDAPI_VULKAN_H

#include "vkworld/VkWorldConfig.h"

#include <vector>
#include <map>
#include <vulkan/vulkan.hpp>

#include <world/core/WorldTypes.h>

#include "VkwEnums.h"
#include "VkwMemoryCache.h"
#include "VkwSubBuffer.h"

namespace world {

/* Naming convention:
 *
 * Call context ctx in all Vulkan helper class
 * vkctx in all World-related class */


class VKWORLD_EXPORT VulkanContext {
public:
    vk::Device _device;

    vk::PhysicalDevice _physicalDevice;

    vk::Queue _computeQueue;

    vk::CommandPool _computeCommandPool;

    vk::DescriptorPool _descriptorPool;


    VulkanContext();

    ~VulkanContext();

    VulkanContext(const VulkanContext &other) = delete;

    VulkanContext &operator=(const VulkanContext &other) = delete;


    // RESOURCES MANAGEMENT
    vk::ShaderModule createShader(const std::vector<char> &shaderCode);

    VkwSubBuffer allocate(u32 size, DescriptorType usage, MemoryType memType);


    // MISC
    /** Simple method to get a queue for computation. */
    int findComputeQueueFamily();

    vk::DescriptorType getDescriptorType(DescriptorType type);

    /** Simple method to find a suitable memory type. */
    u32 findMemoryType(u32 memorySize, vk::MemoryPropertyFlags requiredFlags,
                       vk::MemoryPropertyFlags unwantedFlags = {});

    std::vector<char> readFile(const std::string &filename);


    // TEST
    void configTest();

    void displayAvailableExtensions();

private:
    vk::Instance _instance;

    bool _enableValidationLayers;

    VkDebugReportCallbackEXT _debugCallback;

    typedef std::pair<DescriptorType, MemoryType> memid;
    std::map<memid, std::unique_ptr<VkwMemoryCache>> _memory;


    // INITIALIZATION
    bool checkValidationLayerSupport();

    void setupDebugCallback();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createComputeResources();

    // TODO Remove dat
    friend class ProgramVk;
    friend class BufferVk;
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
