#ifndef VKWORLD_VKWIMAGE_H
#define VKWORLD_VKWIMAGE_H

#include "vkworld/VkWorldConfig.h"

#include <vulkan/vulkan.hpp>

#include <world/core/WorldTypes.h>
#include <world/assets/Image.h>

#include "vkworld/wrappers/IVkwBindable.h"
#include "vkworld/wrappers/IVkwMemoryAccess.h"
#include "VkwEnums.h"

namespace world {

enum class VkwImageUsage {
    TEXTURE,
    OFFSCREEN_RENDER,
};

class VkwImagePrivate {
public:
    vk::Image _image;
    vk::ImageView _imageView;
    vk::Sampler _sampler;
    // TODO Pool memory allocation
    vk::DeviceMemory _memory;

    int _width, _height;
    vk::Format _imageFormat;

    VkwImagePrivate(int width, int height, VkwImageUsage usage,
                    vk::Format format);
    ~VkwImagePrivate();

    void createImageView();

    void createSampler();
};

class VKWORLD_EXPORT VkwImage : public IVkwBindable, public IVkwMemoryAccess {
public:
    static int getMemoryType(VkwImageUsage imgUse, u32 size);

    static void registerArray(std::vector<VkwImage> &imgs,
                              vk::DescriptorSet &descriptorSet,
                              vk::DescriptorType type, u32 id);


    VkwImage(int width, int height,
             VkwImageUsage imgUse = VkwImageUsage::TEXTURE,
             vk::Format format = vk::Format::eR32G32B32A32Sfloat);

    /** Adds this buffer as binding to the given descriptorSet. */
    void registerTo(vk::DescriptorSet &descriptorSet,
                    vk::DescriptorType descriptorType, u32 id) override;

    void setData(const void *data, u32 count, u32 offset) override;

    void getData(void *data, u32 count, u32 offset) override;

    int width() const { return _internal->_width; }

    int height() const { return _internal->_height; }

    vk::Format format() const { return _internal->_imageFormat; }

    vk::SubresourceLayout getSubresourceLayout();

    vk::ImageView getImageView();

private:
    std::shared_ptr<VkwImagePrivate> _internal;
};

} // namespace world

#endif // VKWORLD_VKWIMAGE_H
