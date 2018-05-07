#ifndef WORLDAPI_BUFFERVK_H
#define WORLDAPI_BUFFERVK_H

#include "core/WorldConfig.h"
#include "core/WorldTypes.h"

#include <memory>

#include "Vulkan.h"

namespace world {

enum class DescriptorType {
	STORAGE_BUFFER = 0,
	UNIFORM_BUFFER = 1
};

class PBufferVk;

class WORLDAPI_EXPORT BufferVk {
public:
	BufferVk(VulkanContext &context, DescriptorType descriptorType, u32 size);

	/** Copy "count" bytes from "data" to the buffer. */
	void setData(void* data, u32 count);

	/** Copy "count" bytes from the buffer to the given data array. */
	void getData(void* data, u32 count);

	PBufferVk &internal();
private:
	std::shared_ptr<PBufferVk> _internal;
};

}

#endif // WORLDAPI_BUFFERVK_H