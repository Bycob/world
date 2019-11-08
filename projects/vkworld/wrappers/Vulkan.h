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

    vk::CommandPool _computeCommandPool;
    vk::CommandPool _graphicsCommandPool;

    vk::DescriptorPool _descriptorPool;


    VulkanContext();

    ~VulkanContext();

    VulkanContext(const VulkanContext &other) = delete;

    VulkanContext &operator=(const VulkanContext &other) = delete;


    // RESOURCES MANAGEMENT
    vk::ShaderModule createShader(const std::vector<char> &shaderCode);

    VkwSubBuffer allocate(u32 size, DescriptorType usage, MemoryUsage memType);

    // TODO queue type ?
    vk::Queue queue(vk::QueueFlags flags);


    // MISC
    /** Simple method to get a queue for computation. */
    int findQueueFamily(vk::QueueFlags flags);

    vk::DescriptorType getDescriptorType(DescriptorType type);

    /** Simple method to find a suitable memory type. */
    u32 findMemoryType(u32 memorySize, vk::MemoryPropertyFlags requiredFlags,
                       vk::MemoryPropertyFlags unwantedFlags = {});

    u32 getMemoryType(MemoryUsage memUse, u32 requiredSize);

    vk::SampleCountFlagBits getMaxUsableSampleCount(
        vk::SampleCountFlagBits maxBits = vk::SampleCountFlagBits::e64) const;

    vk::SampleCountFlagBits getDefaultMSAASample() { return _msaaSample; }

    std::vector<char> readFile(const std::string &filename);


    // TEST
    void configTest();

    void displayAvailableExtensions();

private:
    vk::Instance _instance;

    bool _enableValidationLayers;

    VkDebugReportCallbackEXT _debugCallback;

    typedef std::pair<DescriptorType, MemoryUsage> memid;
    std::map<memid, std::unique_ptr<VkwMemoryCache>> _memory;
    vk::SampleCountFlagBits _msaaSample;


    // INITIALIZATION
    bool checkValidationLayerSupport();

    void setupDebugCallback();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createComputeResources();

    void createGraphicsResources();

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
 * - Changer le nom des classes : DescriptorType, MemoryUsage
 * - Passer le VulkanContext en paramètres partout ou nulle part
 * - Destroy resources
 * - register commands on worker a bit less ugly
 */

#endif // WORLDAPI_VULKAN_H
