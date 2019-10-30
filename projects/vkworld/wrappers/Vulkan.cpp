#include "Vulkan.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <chrono>

#include <vulkan/vulkan.h>

#include "world/core/WorldTypes.h"
#include "world/assets/Image.h"


namespace world {

// VULKAN FUNCTIONS

/** debug callback setter */
VkResult CreateDebugReportCallbackEXT(
    VkInstance instance, const VkDebugReportCallbackCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugReportCallbackEXT *pCallback) {
    auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugReportCallbackEXT(VkInstance instance,
                                   VkDebugReportCallbackEXT callback,
                                   const VkAllocationCallbacks *pAllocator) {
    auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugReportCallbackEXT");
    if (func != nullptr) {
        func(instance, callback, pAllocator);
    }
}

// ====


const std::vector<const char *> validationLayers = {
    "VK_LAYER_LUNARG_standard_validation"};

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
              uint64_t obj, size_t location, int32_t code,
              const char *layerPrefix, const char *msg, void *userData) {

    std::cerr << "validation layer: " << msg << std::endl;

    return VK_FALSE;
}

VulkanContext::VulkanContext() {

    // Create instance
    vk::ApplicationInfo appInfo("World", VK_MAKE_VERSION(1, 0, 0), "No Engine",
                                VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

    vk::InstanceCreateInfo createInfo;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char *> extensions;

    // Validation layers
#ifdef _DEBUG
    _enableValidationLayers = true;
#else
    _enableValidationLayers = true; // TODO set it to false when
                                    // debug compilation mode will be
                                    // decently useable
#endif

    if (!checkValidationLayerSupport() && _enableValidationLayers) {
        std::cerr << "[Warning] [Vulkan] Validation layers are not supported"
                  << std::endl;
        _enableValidationLayers = false;
    }

    if (_enableValidationLayers) {
        // Set layer
        createInfo.enabledLayerCount =
            static_cast<u32>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        // Activating suitable extension
        extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    } else {
        createInfo.enabledLayerCount = 0;
    }

    // Setup extensions
    createInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    _instance = vk::createInstance(createInfo);

    // Setup debug callback
    setupDebugCallback();

    // Initialization
    pickPhysicalDevice();
    createLogicalDevice();

    // Compute resources
    createComputeResources();
}

VulkanContext::~VulkanContext() {
    std::cout << "Free all vulkan memory" << std::endl;
    _memory.clear();

    std::cout << "Destroy vulkan ressources" << std::endl;
    _device.destroy(_descriptorPool);
    _device.destroy(_computeCommandPool);
    _device.destroy(_graphicsCommandPool);

    std::cout << "Destroy vulkan device" << std::endl;
    _device.destroy();

    if (_enableValidationLayers) {
        DestroyDebugReportCallbackEXT(_instance, _debugCallback, nullptr);
    }

    // It crashes... why ?
    _instance.destroy();
}

void VulkanContext::displayAvailableExtensions() {
    // Extensions check
    u32 extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensions.data());

    std::cout << "Extensions :" << std::endl;
    for (auto &ext : extensions) {
        std::cout << "\t- " << ext.extensionName << std::endl;
    }
}


// ---- INITIALISATION METHODS

bool VulkanContext::checkValidationLayerSupport() {
    // TODO for each validation layer in validationLayers, check if it's
    // supported and return false if at least one isn't.
    return true;
}

void VulkanContext::setupDebugCallback() {
    VkDebugReportCallbackCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfo.flags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
    createInfo.pfnCallback = debugCallback;

    if (CreateDebugReportCallbackEXT(_instance, &createInfo, nullptr,
                                     &_debugCallback) != VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] [Initialization] failed to setup debug callback");
    }
}

void VulkanContext::pickPhysicalDevice() {
    u32 deviceCount = 0;
    vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("[Vulkan] [Initialization] could not find any "
                                 "device that supports Vulkan");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

    // pick the first device that is suitable
    for (auto &device : devices) {
        // TODO Test suitability
        // Or even better, run a battery of tests to determine which one is the
        // best

        _physicalDevice = device;
        std::cout << "picked device "
                  << _physicalDevice.getProperties().deviceName << std::endl;
        break;
    }
}

void VulkanContext::createLogicalDevice() {
    std::set<int> familyIndexes;
    familyIndexes.insert(findQueueFamily(vk::QueueFlagBits::eGraphics));
    familyIndexes.insert(findQueueFamily(vk::QueueFlagBits::eCompute));

    std::vector<vk::DeviceQueueCreateInfo> queueInfos;

    for (int familyIndex : familyIndexes) {
        float queuePriority = 1.0f;
        queueInfos.push_back(
            vk::DeviceQueueCreateInfo({}, familyIndex, 1, &queuePriority));
    }

    // Device creation
    vk::PhysicalDeviceFeatures deviceFeatures;
    // Enable properties, extensions, etc
    // deviceFeatures.geometryShader = true;

    vk::DeviceCreateInfo createInfo({}, queueInfos.size(), &queueInfos[0]);
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

    // Validation layers
    if (_enableValidationLayers) {
        createInfo.enabledLayerCount =
            static_cast<u32>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    _device = _physicalDevice.createDevice(createInfo, nullptr);
}

void VulkanContext::createComputeResources() {
    // Create descriptor pool
    // TODO Scale descriptor pool size according to needs. Change the system if
    // it's no more sufficient.
    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;
    descriptorPoolSizes.resize(3);

    descriptorPoolSizes[0].type = vk::DescriptorType::eStorageBuffer;
    descriptorPoolSizes[0].descriptorCount = 2000;

    descriptorPoolSizes[1].type = vk::DescriptorType::eUniformBuffer;
    descriptorPoolSizes[1].descriptorCount = 2000;

    descriptorPoolSizes[2].type = vk::DescriptorType::eSampledImage;
    descriptorPoolSizes[2].descriptorCount = 2000;

    vk::DescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.maxSets = 750;
    descriptorPoolInfo.flags |=
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
    descriptorPoolInfo.poolSizeCount = descriptorPoolSizes.size();
    descriptorPoolInfo.pPoolSizes = &descriptorPoolSizes[0];

    _descriptorPool = _device.createDescriptorPool(descriptorPoolInfo, nullptr);

    // Create command pools
    vk::CommandPoolCreateInfo computePoolInfo(
        {}, findQueueFamily(vk::QueueFlagBits::eCompute));
    _computeCommandPool = _device.createCommandPool(computePoolInfo, nullptr);

    vk::CommandPoolCreateInfo graphicsPoolInfo(
        {}, findQueueFamily(vk::QueueFlagBits::eGraphics));
    _graphicsCommandPool = _device.createCommandPool(graphicsPoolInfo, nullptr);
}

vk::ShaderModule VulkanContext::createShader(
    const std::vector<char> &shaderCode) {
    vk::ShaderModuleCreateInfo shaderInfo(
        {}, shaderCode.size(),
        reinterpret_cast<const u32 *>(shaderCode.data()));
    vk::ShaderModule shader = _device.createShaderModule(shaderInfo);

    // Add to the internal register for further deleting
    return shader;
}

int VulkanContext::findQueueFamily(vk::QueueFlags flags) {
    // Get queues family
    std::vector<vk::QueueFamilyProperties> queueFamilies =
        _physicalDevice.getQueueFamilyProperties();

    int i = 0;
    for (auto &queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & flags) {
            return i;
        }

        ++i;
    }

    return -1;
}

vk::DescriptorType VulkanContext::getDescriptorType(DescriptorType usage) {
    switch (usage) {
    case DescriptorType::STORAGE_BUFFER:
        return vk::DescriptorType::eStorageBuffer;
    case DescriptorType::UNIFORM_BUFFER:
        return vk::DescriptorType::eUniformBuffer;
    default:
        throw std::invalid_argument("unkown descriptor usage");
    }
}

u32 VulkanContext::findMemoryType(u32 memorySize,
                                  vk::MemoryPropertyFlags requiredFlags,
                                  vk::MemoryPropertyFlags unwantedFlags) {
    vk::PhysicalDeviceMemoryProperties properties =
        _physicalDevice.getMemoryProperties();

    u32 memoryTypeIndex = 0;
    bool foundMemoryType = false;

    for (u32 i = 0; i < properties.memoryTypeCount; ++i) {
        const vk::MemoryType memoryType = properties.memoryTypes[i];

        if ((requiredFlags & memoryType.propertyFlags) == requiredFlags &&
            (~unwantedFlags | memoryType.propertyFlags) == ~unwantedFlags &&
            (properties.memoryHeaps[memoryType.heapIndex].size > memorySize)) {
            memoryTypeIndex = i;
            foundMemoryType = true;
            break;
        }
    }

    if (!foundMemoryType) {
        throw std::runtime_error(
            "[Vulkan] [Config-Test] did not find suitable memory type");
    }

    return memoryTypeIndex;
}

VkwSubBuffer VulkanContext::allocate(u32 size, DescriptorType usage,
                                     MemoryType memType) {
    memid key(usage, memType);
    auto it = _memory.find(key);

    if (it == _memory.end()) {
        // segment size = 64 Mo
        it = _memory
                 .insert(
                     std::make_pair(key, std::make_unique<VkwMemoryCache>(
                                             64 * 1024 * 1024, usage, memType)))
                 .first;
    }

    auto &allocator = it->second;
    return allocator->allocateBuffer(size);
}

vk::Queue VulkanContext::queue(vk::QueueFlags flags) {
    return _device.getQueue(findQueueFamily(flags), 0);
}

std::vector<char> VulkanContext::readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.good()) {
        throw std::runtime_error("[Vulkan] [readFile] File not found");
    }

    size_t filesize = static_cast<size_t>(file.tellg());
    std::vector<char> result(filesize);
    file.seekg(0, file.beg);
    file.read(result.data(), filesize);
    file.close();
    return result;
}


VulkanContext &Vulkan::context() {
    static VulkanContext context;
    return context;
}

// http://www.duskborn.com/a-simple-vulkan-compute-example/
// https://github.com/Erkaman/vulkan_minimal_compute
/** Simple perlin noise throught a 1024 * 1024 image */
void VulkanContext::configTest() {
    // TODO Currently this is a kind of toy to play around with Vulkan.
    // Later we should split the differents parts of this function into useable
    // methods for the API.

    const VkDeviceSize memorySize = 1024 * 1024 * 4 * sizeof(float);

    // Creating buffer
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = memorySize;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer;
    vkCreateBuffer(_device, &bufferInfo, nullptr, &buffer);

    // Allocate memory
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);

    VkMemoryAllocateInfo memoryInfo = {};
    memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryInfo.pNext = nullptr;
    memoryInfo.allocationSize = memRequirements.size;
    memoryInfo.memoryTypeIndex = findMemoryType(
        memorySize, vk::MemoryPropertyFlagBits::eHostVisible |
                        vk::MemoryPropertyFlagBits::eHostCoherent);

    VkDeviceMemory memory;
    if (vkAllocateMemory(_device, &memoryInfo, nullptr, &memory) !=
        VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] [Config-Test] failed allocate memory");
    }

    // Bind memory
    vkBindBufferMemory(_device, buffer, memory, 0);

    // ==========

    // Create descriptor set layout
    VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
    descriptorSetLayoutBinding.binding = 0;
    descriptorSetLayoutBinding.descriptorType =
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount =
        1; // only a single binding in this descriptor set layout.
    descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(_device, &descriptorSetLayoutCreateInfo,
                                    NULL, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] [Config-Test] failed create descriptor set layout");
    }

    // Allocate descriptor set in the pool
    VkDescriptorSetAllocateInfo descriptorSetInfo = {};
    descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetInfo.descriptorPool = _descriptorPool;
    descriptorSetInfo.descriptorSetCount = 1;
    descriptorSetInfo.pSetLayouts = &descriptorSetLayout;

    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(_device, &descriptorSetInfo, &descriptorSet) !=
        VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] [Config-Test] failed allocate descriptor set");
    }


    // Connect buffer to descriptor
    VkDescriptorBufferInfo descriptorBufferInfo = {};
    descriptorBufferInfo.buffer = buffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = memorySize;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSet; // write to this descriptor set.
    writeDescriptorSet.dstBinding = 0; // write to the first, and only binding.
    writeDescriptorSet.descriptorCount = 1; // update a single descriptor.
    writeDescriptorSet.descriptorType =
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // storage buffer.
    writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

    vkUpdateDescriptorSets(_device, 1, &writeDescriptorSet, 0, nullptr);

    // ==========

    // Setup the computation pipeline
    // Create shader
    auto shader = createShader(readFile("perlin.spv"));

    // Create pipeline stage info
    VkPipelineShaderStageCreateInfo pipelineStageInfo = {};
    pipelineStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineStageInfo.module = shader;
    pipelineStageInfo.pName = "main";

    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    VkPipelineLayout pipelineLayout;
    if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr,
                               &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] [Config-Test] failed create pipeline layout");
    }

    // Create pipeline from layout and stage info
    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stage = pipelineStageInfo;
    pipelineCreateInfo.layout = pipelineLayout;

    VkPipeline pipeline;
    if (vkCreateComputePipelines(_device, VK_NULL_HANDLE, 1,
                                 &pipelineCreateInfo, nullptr,
                                 &pipeline) != VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] [Config-Test] failed create pipeline");
    }

    // ==========

    // Create command buffer
    VkCommandPoolCreateInfo commandPoolCreateInfo = {};
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolCreateInfo.flags = 0;
    commandPoolCreateInfo.queueFamilyIndex =
        findQueueFamily(vk::QueueFlagBits::eCompute);

    VkCommandPool commandPool;
    if (vkCreateCommandPool(_device, &commandPoolCreateInfo, nullptr,
                            &commandPool) != VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] [Config-Test] failed create command pool");
    }

    // Allocate a command buffer
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    VkCommandBuffer commandBuffer;
    // TODO Test VK_SUCCESS
    vkAllocateCommandBuffers(_device, &commandBufferAllocateInfo,
                             &commandBuffer);


    // Writing the commands
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // the buffer
                                                                   // is only
    // submitted and used once
    // in this application.

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                            pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

    // Dispatching the working groups
    vkCmdDispatch(commandBuffer, 32, 32, 1);
    // End recording
    // TODO Test VK_SUCCESS
    vkEndCommandBuffer(commandBuffer);

    // ==========

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1; // submit a single command buffer
    submitInfo.pCommandBuffers = &commandBuffer; // the command buffer to
    // submit.

    // Create fence
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;

    VkFence fence;
    // TODO Test VK_SUCCESS
    vkCreateFence(_device, &fenceCreateInfo, NULL, &fence);

    // Submid queue
    // TODO Test VK_SUCCESS
    vkQueueSubmit(queue(vk::QueueFlagBits::eCompute), 1, &submitInfo, fence);

    auto start = std::chrono::steady_clock::now();
    // TODO Test VK_SUCCESS
    vkWaitForFences(_device, 1, &fence, VK_TRUE, 100000000000);
    std::cout << "Exploration terminee en "
              << std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::steady_clock::now() - start)
                     .count()
              << " us" << std::endl;

    vkDestroyFence(_device, fence, NULL);

    // ==========
    start = std::chrono::steady_clock::now();

    // Read memory
    float *mappedMemory = nullptr;
    vkMapMemory(_device, memory, 0, memorySize, 0,
                reinterpret_cast<void **>(&mappedMemory));

    // Fill and save image
    Image image(1024, 1024, ImageType::RGBA);

    // TODO make a method to fill buffer directly
    for (u32 y = 0; y < 1024; ++y) {
        for (u32 x = 0; x < 1024; ++x) {
            float *ptr = &(mappedMemory[(y * 1024 + x) * 4]);
            image.rgba(x, y).setf(ptr[0], ptr[1], ptr[2], ptr[3]);
        }
    }
    std::cout << "Exploration terminee en "
              << std::chrono::duration_cast<std::chrono::microseconds>(
                     std::chrono::steady_clock::now() - start)
                     .count()
              << " us" << std::endl;

    image.write("config-test.png");

    vkUnmapMemory(_device, memory);
}

} // namespace world
