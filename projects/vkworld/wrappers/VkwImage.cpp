#include "VkwImage.h"

#include "Vulkan.h"

namespace world {

VkwImagePrivate::VkwImagePrivate(int width, int height)
        : _width(width), _height(height) {
    auto &ctx = Vulkan::context();

    // TODO use correct format
    _imageFormat = vk::Format::eUndefined;

    // Create the image
    // TODO: Check what ImageUsage really means
    vk::ImageCreateInfo imageInfo(
        {}, vk::ImageType::e2D, _imageFormat,
        {static_cast<u32>(_width), static_cast<u32>(_height), 1}, 1, 1,
        vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eSampled);
    _image = ctx._device.createImage(imageInfo);

    // Create the image view
    vk::ImageViewCreateInfo imageViewInfo({}, _image, vk::ImageViewType::e2D,
                                          _imageFormat);
    imageViewInfo.components.r = imageViewInfo.components.g =
        imageViewInfo.components.b = imageViewInfo.components.a =
            vk::ComponentSwizzle::eIdentity;
    imageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    _imageView = ctx._device.createImageView(imageViewInfo);

    vk::SamplerCreateInfo samplerInfo(
        {}, vk::Filter::eLinear, vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eClampToEdge,
        vk::SamplerAddressMode::eClampToEdge,
        vk::SamplerAddressMode::eClampToEdge, 0.0f, VK_FALSE, 1.0f, VK_FALSE,
        vk::CompareOp::eAlways, 0.0f, 1.0f, vk::BorderColor::eFloatOpaqueWhite);
    _sampler = ctx._device.createSampler(samplerInfo);

    vk::MemoryRequirements memRequirements =
        ctx._device.getImageMemoryRequirements(_image);
    vk::MemoryAllocateInfo memAllocate(memRequirements.size,
                                       memRequirements.alignment);
    memAllocate.memoryTypeIndex = 1; // FIXME Memory type

    _memory = ctx._device.allocateMemory(memAllocate);
}

VkwImagePrivate::~VkwImagePrivate() {
    auto &ctx = Vulkan::context();

    ctx._device.destroy(_imageView);
    ctx._device.destroy(_sampler);
    ctx._device.destroy(_image);
    ctx._device.free(_memory);
}

VkwImage::VkwImage(int width, int height)
        : _internal(std::make_shared<VkwImagePrivate>(width, height)) {}

void VkwImage::registerTo(vk::DescriptorSet &descriptorSet,
                          vk::DescriptorType descriptorType, u32 id) {
    vk::DescriptorImageInfo descriptorImageInfo(_internal->_sampler,
                                                _internal->_imageView,
                                                vk::ImageLayout::eUndefined);
    vk::WriteDescriptorSet writeDescriptorSet(
        descriptorSet, id, 0, 1, descriptorType, &descriptorImageInfo);

    auto &ctx = Vulkan::context();
    ctx._device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

void VkwImage::setData(void *data, u32 count, u32 offset) {
    VulkanContext &ctx = Vulkan::context();

    void *mapped = ctx._device.mapMemory(_internal->_memory, offset, count);
    memcpy(mapped, data, count);
    ctx._device.unmapMemory(_internal->_memory);
}

void VkwImage::getData(void *data, u32 count, u32 offset) {
    VulkanContext &ctx = Vulkan::context();

    void *mapped = ctx._device.mapMemory(_internal->_memory, offset, count);
    memcpy(data, mapped, count);
    ctx._device.unmapMemory(_internal->_memory);
}

vk::ImageView VkwImage::getImageView() { return _internal->_imageView; }

} // namespace world
