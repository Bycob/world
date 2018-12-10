#ifndef WORLDAPI_PVULKAN_H
#define WORLDAPI_PVULKAN_H

#include <vulkan.h>
#include <vector>

#include <world/core/WorldTypes.h>

#include "Vulkan.h"

namespace world {

class PVulkanContext {
public:
	VkInstance _instance;

	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

	VkDevice _device;

	VkQueue _computeQueue;

	VkCommandPool _computeCommandPool;

	VkDescriptorPool _descriptorPool;

	bool _enableValidationLayers;

	VkDebugReportCallbackEXT _debugCallback;


	// INITIALIZATION
	bool checkValidationLayerSupport();

	void setupDebugCallback();

	void pickPhysicalDevice();

	void createLogicalDevice();

	void createComputeResources();


	// RESOURCES MANAGEMENT
	VkShaderModule createShader(const std::vector<char> &shaderCode);

	// MISC
	/** Simple method to get a queue for computation. */
	int findComputeQueueFamily();

	/** Simple method to find a suitable memory type. */
	int findMemoryType(u32 memorySize);

	std::vector<char> readFile(const std::string &filename);
};

}

#endif // WORLDAPI_PVULKAN_H
