#ifndef WORLDAPI_PVULKAN_H
#define WORLDAPI_PVULKAN_H

#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>

#include <world/core/WorldTypes.h>

#include "Vulkan.h"
#include "VkEnums.h"
#include "VkMemoryCache.h"
#include "VkSubBuffer.h"

namespace world {

class VulkanContextPrivate {
public:
    vk::Instance _instance;

    vk::PhysicalDevice _physicalDevice;

    vk::Device _device;

    vk::Queue _computeQueue;

    vk::CommandPool _computeCommandPool;

    vk::DescriptorPool _descriptorPool;


    // INITIALIZATION
    bool checkValidationLayerSupport();

    void setupDebugCallback();

    void pickPhysicalDevice();

    void createLogicalDevice();

    void createComputeResources();


    // RESOURCES MANAGEMENT
    vk::ShaderModule createShader(const std::vector<char> &shaderCode);

    VkSubBuffer allocate(u32 size, DescriptorType usage, MemoryType memType);

    // MISC
    /** Simple method to get a queue for computation. */
    int findComputeQueueFamily();

    vk::DescriptorType getDescriptorType(DescriptorType type);

    /** Simple method to find a suitable memory type. */
    int findMemoryType(u32 memorySize, vk::MemoryPropertyFlags requiredFlags,
                       vk::MemoryPropertyFlags unwantedFlags = {});

    std::vector<char> readFile(const std::string &filename);

private:
    bool _enableValidationLayers;

    VkDebugReportCallbackEXT _debugCallback;

    std::map<std::pair<DescriptorType, MemoryType>, VkMemoryCache> _memory;


    friend class VulkanContext;
};

} // namespace world

#endif // WORLDAPI_PVULKAN_H
