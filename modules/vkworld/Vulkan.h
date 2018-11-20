#ifndef WORLDAPI_VULKAN_H
#define WORLDAPI_VULKAN_H

#include "VkWorldConfig.h"

namespace world {
class PVulkanContext;

class VKWORLD_EXPORT VulkanContext {
public:
	VulkanContext();

	~VulkanContext();

	void configTest();

	void displayAvailableExtensions();

	PVulkanContext &internal();
private:
	PVulkanContext *_internal;
};

struct VKWORLD_EXPORT Vulkan {
	static VulkanContext &context();
};

}

#endif // WORLDAPI_VULKAN_H
