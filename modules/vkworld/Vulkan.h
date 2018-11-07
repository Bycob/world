#ifndef WORLDAPI_VULKAN_H
#define WORLDAPI_VULKAN_H

#include "core/WorldConfig.h"

namespace world {
class PVulkanContext;

class WORLDAPI_EXPORT VulkanContext {
public:
	VulkanContext();

	~VulkanContext();

	void configTest();

	void displayAvailableExtensions();

	PVulkanContext &internal();
private:
	PVulkanContext *_internal;
};

struct WORLDAPI_EXPORT Vulkan {
	static VulkanContext &context();
};

}

#endif // WORLDAPI_VULKAN_H
