#include "VkwImage.h"

#include <iostream>

#include "Vulkan.h"

namespace world {

VkwImagePrivate::VkwImagePrivate(int width, int height, VkwImageUsage imgUse,
                                 vk::Format format)
        : _width(width), _height(height), _imageFormat(format) {
    auto &ctx = Vulkan::context();

    // Create the image
    vk::ImageUsageFlags imgUsageBits;
    switch (imgUse) {
    case VkwImageUsage::TEXTURE:
        imgUsageBits |= vk::ImageUsageFlagBits::eSampled;
        break;
    case VkwImageUsage::OFFSCREEN_RENDER:
        imgUsageBits |= vk::ImageUsageFlagBits::eColorAttachment |
                        vk::ImageUsageFlagBits::eTransferSrc;
        break;
    }
    // What is color attachment
    // What is transfer Dst

    vk::ImageCreateInfo imageInfo(
        {}, vk::ImageType::e2D, _imageFormat,
        {static_cast<u32>(_width), static_cast<u32>(_height), 1}, 1, 1,
        vk::SampleCountFlagBits::e1, vk::ImageTiling::eLinear, imgUsageBits);
    imageInfo.initialLayout = vk::ImageLayout::ePreinitialized;
    _image = ctx._device.createImage(imageInfo);

    // Allocate memory for the image
    vk::MemoryRequirements memRequirements =
        ctx._device.getImageMemoryRequirements(_image);
    vk::MemoryAllocateInfo memAllocate(memRequirements.size,
                                       memRequirements.alignment);
    memAllocate.memoryTypeIndex =
        VkwImage::getMemoryType(imgUse, memRequirements.size);

    _memory = ctx._device.allocateMemory(memAllocate);
    ctx._device.bindImageMemory(_image, _memory, 0);

    // Create the image view
    vk::ImageViewCreateInfo imageViewInfo({}, _image, vk::ImageViewType::e2D,
                                          _imageFormat);
    imageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    _imageView = ctx._device.createImageView(imageViewInfo);

    vk::SamplerCreateInfo samplerInfo(
        {}, vk::Filter::eLinear, vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat, vk::SamplerAddressMode::eClampToEdge,
        0.0f, VK_FALSE, 1.0f, VK_FALSE, vk::CompareOp::eAlways, 0.0f, 1.0f,
        vk::BorderColor::eFloatOpaqueWhite);
    _sampler = ctx._device.createSampler(samplerInfo);
}

VkwImagePrivate::~VkwImagePrivate() {
    auto &ctx = Vulkan::context();

    ctx._device.destroy(_imageView);
    ctx._device.destroy(_sampler);
    ctx._device.destroy(_image);
    ctx._device.free(_memory);
}


int VkwImage::getMemoryType(VkwImageUsage imgUse, u32 size) {
    auto &ctx = Vulkan::context();
    MemoryUsage memUse = imgUse == VkwImageUsage::TEXTURE
                             ? MemoryUsage::CPU_WRITES
                             : MemoryUsage::CPU_READS;
    return ctx.getMemoryType(memUse, size);
}

VkwImage::VkwImage(int width, int height, VkwImageUsage imgUse,
                   vk::Format format)
        : _internal(std::make_shared<VkwImagePrivate>(width, height, imgUse,
                                                      format)) {}

void VkwImage::registerTo(vk::DescriptorSet &descriptorSet,
                          vk::DescriptorType descriptorType, u32 id) {
    vk::DescriptorImageInfo descriptorImageInfo(
        _internal->_sampler, _internal->_imageView, vk::ImageLayout::eGeneral);
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
