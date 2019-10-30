#ifndef VKWORLD_VKWIMAGE_H
#define VKWORLD_VKWIMAGE_H

#include "vkworld/VkWorldConfig.h"

#include <vulkan/vulkan.hpp>

#include <world/core/WorldTypes.h>

#include "vkworld/wrappers/IVkwBindable.h"
#include "vkworld/wrappers/IVkwMemoryAccess.h"

namespace world {

class VkwImagePrivate {
public:
    vk::Image _image;
    vk::ImageView _imageView;
    vk::Sampler _sampler;
    // TODO Pool memory allocation
    vk::DeviceMemory _memory;

    int _width, _height;
    vk::Format _imageFormat;

    VkwImagePrivate(int width, int height);
    ~VkwImagePrivate();
};

class VKWORLD_EXPORT VkwImage : public IVkwBindable, public IVkwMemoryAccess {
public:
    VkwImage(int width = 1, int height = 1);

    /** Adds this buffer as binding to the given descriptorSet. */
    void registerTo(vk::DescriptorSet &descriptorSet,
                    vk::DescriptorType descriptorType, u32 id) override;

    void setData(void *data, u32 count, u32 offset) override;

    void getData(void *data, u32 count, u32 offset) override;

    int width() const { return _internal->_width; }

    int height() const { return _internal->_height; }

    vk::ImageView getImageView();

private:
    std::shared_ptr<VkwImagePrivate> _internal;
};

} // namespace world

#endif // VKWORLD_VKWIMAGE_H
