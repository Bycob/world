#ifdef WORLD_BUILD_VULKAN_MODULES
#include "Vulkan.h"

#include <iostream>
#include <vector>

#include <vulkan.h>

#include "core/WorldTypes.h"

// VULKAN FUNCTIONS

/** debug callback setter */
VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	auto func = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

// ====

namespace world {

class PVulkanContext {
public:
	VkInstance _instance;

	VkPhysicalDevice _physicalDevice = VK_NULL_HANDLE;

	VkDevice _device;

	VkQueue _queue;

	bool _enableValidationLayers;

	VkDebugReportCallbackEXT _debugCallback;


	bool checkValidationLayerSupport();

	void setupDebugCallback();

	void pickPhysicalDevice();

	void createLogicalDevice();

	/** Simple method to get a queue for computation */
	int findQueueFamily(VkPhysicalDevice device);
};

const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}

// ---- INITIALISATION METHODS

void PVulkanContext::setupDebugCallback() {
	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (CreateDebugReportCallbackEXT(_instance, &createInfo, nullptr, &_debugCallback) != VK_SUCCESS) {
		throw std::runtime_error("[Vulkan] [Initialisation] failed to setup debug callback");
	}
}

void PVulkanContext::pickPhysicalDevice() {
	u32 deviceCount = 0;
	vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("[Vulkan] [Initialisation] could not find any device that supports Vulkan");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());

	// pick the first device that is suitable
	for (auto &device : devices) {
		// TODO Test suitability
		// Or even better, run a battery of tests to determine which one is the best

		_physicalDevice = device;
	}
}

void PVulkanContext::createLogicalDevice() {
	int familyIndex = findQueueFamily(_physicalDevice);

	// Information about the queue
	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = familyIndex;
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	// Apparently Vulkan makes a copy of the value, and that's why we pass a temporary value
	// Discussion about that here : https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Logical_device_and_queues
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkPhysicalDeviceFeatures deviceFeatures = {};

	// Device creation
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;

	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = 0;

	// Validation layers
	if (_enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<u32>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS) {
		throw std::runtime_error("[Vulkan] [Initialisation] failed to create logical device!");
	}

	// Get queue
	vkGetDeviceQueue(_device, familyIndex, 0, &_queue);
}

bool PVulkanContext::checkValidationLayerSupport() {
	// TODO for each validation layer in validationLayers, check if it's supported and return false if at least one isn't.
	return true;
}

int PVulkanContext::findQueueFamily(VkPhysicalDevice device) {
	// Get queues family
	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (auto &queueFamily : queueFamilies) {
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			return i;
		}

		++i;
	}

	return -1;
}

// ---- PUBLIC METHODS

VulkanContext::VulkanContext()
	: _internal(new PVulkanContext()) {

	// Create instance
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "World";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.pNext = nullptr;

	std::vector<const char*> extensions;

	// Validation layers
#ifdef _DEBUG
	_internal->_enableValidationLayers = true;
#else
	_internal->_enableValidationLayers = true; // TODO set it to false when debug will be decently useable
#endif

	if (!_internal->checkValidationLayerSupport() && _internal->_enableValidationLayers) {
		std::cerr << "[Warning] [Vulkan] Validation layers are not supported" << std::endl;
		_internal->_enableValidationLayers = false;
	}

	if (_internal->_enableValidationLayers) {
		// Set layer
		createInfo.enabledLayerCount = static_cast<u32>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		// Activating suitable extension
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	// Setup extensions
	createInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (vkCreateInstance(&createInfo, nullptr, &_internal->_instance) != VK_SUCCESS) {
		throw std::runtime_error("[Vulkan] [Initialisation] failed to create instance!");
	}

	// Setup debug callback
	_internal->setupDebugCallback();

	// Initialisation
	_internal->pickPhysicalDevice();
	_internal->createLogicalDevice();
}

VulkanContext::~VulkanContext() {
	vkDestroyDevice(_internal->_device, nullptr);

	if (_internal->_enableValidationLayers) {
		DestroyDebugReportCallbackEXT(_internal->_instance, _internal->_debugCallback, nullptr);
	}

	// It crashes... why ?
	vkDestroyInstance(_internal->_instance, nullptr);

	delete _internal;
}

// http://www.duskborn.com/a-simple-vulkan-compute-example/
/** Simple perlin noise throught a 1024 * 1024 image */
void VulkanContext::configTest() {
	// TODO Currently this is a kind of toy to play around with Vulkan.
	// Later we should split the differents parts of this function into useable methods for the API.
	
	const VkDeviceSize memorySize = 1024 * 1024 * 3;
	
	// Creating buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = memorySize;
	bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkBuffer buffer;
	vkCreateBuffer(_internal->_device, &bufferInfo, nullptr, &buffer);

	// Allocate memory
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(_internal->_device, buffer, &memRequirements);

	VkMemoryAllocateInfo memoryInfo = {};
	memoryInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryInfo.pNext = nullptr;
	memoryInfo.allocationSize = memRequirements.size;

	// Find memory type for the needed size
	VkPhysicalDeviceMemoryProperties properties;
	vkGetPhysicalDeviceMemoryProperties(_internal->_physicalDevice, &properties);

	u32 memoryTypeIndex;

	for (u32 i = 0; i < properties.memoryTypeCount; ++i) {
		const VkMemoryType memoryType = properties.memoryTypes[i];

		if (((VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) & memoryType.propertyFlags) != 0
			&& (properties.memoryHeaps[memoryType.heapIndex].size < memorySize)) {

			memoryTypeIndex = i;
		}
	}

	memoryInfo.memoryTypeIndex = memoryTypeIndex;

	VkDeviceMemory memory;
	vkAllocateMemory(_internal->_device, &memoryInfo, nullptr, &memory);

	// Bind memory
	vkBindBufferMemory(_internal->_device, buffer, memory, 0);
}

void VulkanContext::displayAvailableExtensions() {
	// Extensions check
	u32 extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	std::cout << "Extensions :" << std::endl;
	for (auto &ext : extensions) {
		std::cout << "\t- " << ext.extensionName << std::endl;
	}
}


VulkanContext &Vulkan::context() {
	static VulkanContext context;
	return context;
}
}

#endif