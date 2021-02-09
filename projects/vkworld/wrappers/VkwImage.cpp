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
        imgUsageBits |= vk::ImageUsageFlagBits::eSampled |
                        vk::ImageUsageFlagBits::eTransferDst;
        break;
    case VkwImageUsage::OFFSCREEN_RENDER:
        imgUsageBits |= vk::ImageUsageFlagBits::eColorAttachment |
                        vk::ImageUsageFlagBits::eTransferSrc;
        // TODO find how to convert rendered image into sampled
        break;
    }
    // What is color attachment
    // What is transfer Dst

    vk::ImageCreateInfo imageInfo(
        {}, vk::ImageType::e2D, _imageFormat,
        {static_cast<u32>(_width), static_cast<u32>(_height), 1}, 1, 1,
        vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal, imgUsageBits);
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    _image = ctx._device.createImage(imageInfo);

    // Allocate memory for the image
    vk::MemoryRequirements memRequirements =
        ctx._device.getImageMemoryRequirements(_image);
    // VkwImage::getMemoryType(imgUse, memRequirements.size);

    vk::MemoryAllocateInfo memAllocate(memRequirements.size,
                                       memRequirements.alignment);
    memAllocate.memoryTypeIndex = ctx.findMemoryType(
        memRequirements.size, vk::MemoryPropertyFlagBits::eDeviceLocal, {}, memRequirements.memoryTypeBits);

    _memory = ctx._device.allocateMemory(memAllocate);
    ctx._device.bindImageMemory(_image, _memory, 0);
}

VkwImagePrivate::~VkwImagePrivate() {
    auto &ctx = Vulkan::context();

    if (_imageView)
        ctx._device.destroy(_imageView);
    if (_sampler)
        ctx._device.destroy(_sampler);
    ctx._device.destroy(_image);
    ctx._device.free(_memory);
}

void VkwImagePrivate::createImageView() {
    auto &ctx = Vulkan::context();

    vk::ImageViewCreateInfo imageViewInfo({}, _image, vk::ImageViewType::e2D,
                                          _imageFormat);
    imageViewInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;
    _imageView = ctx._device.createImageView(imageViewInfo);
}

void VkwImagePrivate::createSampler() {
    auto &ctx = Vulkan::context();

    vk::SamplerCreateInfo samplerInfo(
        {}, vk::Filter::eLinear, vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear, _addressX, _addressY,
        vk::SamplerAddressMode::eClampToEdge, 0.0f, VK_FALSE, 1.0f, VK_FALSE,
        vk::CompareOp::eAlways, 0.0f, 1.0f, vk::BorderColor::eFloatOpaqueWhite);
    _sampler = ctx._device.createSampler(samplerInfo);
}


int VkwImage::getMemoryType(VkwImageUsage imgUse, u32 size) {
    auto &ctx = Vulkan::context();
    MemoryUsage memUse = imgUse == VkwImageUsage::TEXTURE
                             ? MemoryUsage::CPU_WRITES
                             : MemoryUsage::CPU_READS;
    return ctx.getMemoryType(memUse, size);
}

void VkwImage::registerArray(std::vector<VkwImage> &imgs,
                             vk::DescriptorSet &descriptorSet,
                             vk::DescriptorType type, u32 id) {
    std::vector<vk::DescriptorImageInfo> imgInfos;
    imgInfos.reserve(imgs.size());

    for (VkwImage &img : imgs) {
        imgInfos.emplace_back(img.getSampler(), img.getImageView(),
                              vk::ImageLayout::eGeneral);
    }

    vk::WriteDescriptorSet writeDescriptorSet(descriptorSet, id, 0, imgs.size(),
                                              type, &imgInfos[0]);
    auto &ctx = Vulkan::context();
    ctx._device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

VkwImage::VkwImage(int width, int height, VkwImageUsage imgUse,
                   vk::Format format)
        : _internal(std::make_shared<VkwImagePrivate>(width, height, imgUse,
                                                      format)) {}

void VkwImage::setSamplerAddressMode(vk::SamplerAddressMode addMode) {
    setSamplerAddressMode(addMode, addMode);
}

void VkwImage::setSamplerAddressMode(vk::SamplerAddressMode addModeX,
                                     vk::SamplerAddressMode addModeY) {
    _internal->_addressX = addModeX;
    _internal->_addressY = addModeY;
}

void VkwImage::registerTo(vk::DescriptorSet &descriptorSet,
                          vk::DescriptorType descriptorType, u32 id) {
    vk::DescriptorImageInfo descriptorImageInfo(getSampler(), getImageView(),
                                                vk::ImageLayout::eGeneral);
    vk::WriteDescriptorSet writeDescriptorSet(
        descriptorSet, id, 0, 1, descriptorType, &descriptorImageInfo);

    auto &ctx = Vulkan::context();
    ctx._device.updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
}

// TODO move this function out in utility somewhere
void insertBarrier(vk::CommandBuffer commandBuf,
    vk::Image image,
    vk::AccessFlags srcAccessMask,
    vk::AccessFlags dstAccessMask,
    vk::ImageLayout srcLayout,
    vk::ImageLayout dstLayout,
    vk::PipelineStageFlags srcStageMask,
    vk::PipelineStageFlags dstStageMask) {

    vk::ImageMemoryBarrier memBarrier(srcAccessMask, dstAccessMask, srcLayout,
        dstLayout);
    memBarrier.image = image;
    memBarrier.subresourceRange =
        vk::ImageSubresourceRange(vk::ImageAspectFlagBits ::eColor, 0, 1, 0, 1);
    commandBuf.pipelineBarrier(srcStageMask, dstStageMask, {}, {}, {},
        memBarrier);
}

// TODO update this function
void VkwImage::setData(const void *data, u32 count, u32 offset) {
    VulkanContext &ctx = Vulkan::context();

    VkwSubBuffer staging = ctx.allocate(count, DescriptorType::TRANSFER_SRC,
                                        MemoryUsage::CPU_WRITES);
    staging.setData(data, count, offset);

    // Copy staging buffer to image
    vk::CommandBufferAllocateInfo commandBufInfo(
        ctx._graphicsCommandPool, vk::CommandBufferLevel::ePrimary, 1);
    auto commandBuf = ctx._device.allocateCommandBuffers(commandBufInfo).at(0);
    commandBuf.begin(vk::CommandBufferBeginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

    /*insertBarrier(commandBuf, stagingImg, {}, vk::AccessFlagBits ::eTransferWrite,
                  vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral,
                  vk::PipelineStageFlagBits::eTopOfPipe,
                  vk::PipelineStageFlagBits::eTransfer);*/

    vk::BufferImageCopy imgCopy{
        staging.getOffset(),
        0u,
        0u,
        vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1),
        vk::Offset3D{0, 0, 0},
        vk::Extent3D{static_cast<u32>(_internal->_width),
                     static_cast<u32>(_internal->_height), 1u}};
    commandBuf.copyBufferToImage(staging.handle(), _internal->_image,
                                 vk::ImageLayout::eGeneral, imgCopy);

    /*insertBarrier(commandBuf, stagingImg, vk::AccessFlagBits ::eTransferWrite,
                  vk::AccessFlagBits ::eShaderRead, vk::ImageLayout::eGeneral,
                  vk::ImageLayout::eGeneral,
                  vk::PipelineStageFlagBits::eTransfer,
                  vk::PipelineStageFlagBits::eFragmentShader);*/

    commandBuf.end();

    auto fence = ctx._device.createFence(vk::FenceCreateInfo());
    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &commandBuf);
    ctx.queue(vk::QueueFlagBits::eGraphics).submit(submitInfo, fence);

    ctx._device.waitForFences(fence, VK_TRUE, UINT64_MAX);

    // Destroy resources
    ctx._device.freeCommandBuffers(ctx._graphicsCommandPool, commandBuf);
    ctx._device.destroyFence(fence);
}

void VkwImage::getData(void *data, u32 count, u32 offset) {
    VulkanContext &ctx = Vulkan::context();

    vk::ImageCreateInfo stagingImgInfo(
        {}, vk::ImageType::e2D, _internal->_imageFormat,
        vk::Extent3D(static_cast<u32>(_internal->_width), static_cast<u32>(_internal->_height), 1u), 1u, 1u,
        vk::SampleCountFlagBits::e1, vk::ImageTiling::eLinear,
        vk::ImageUsageFlagBits::eTransferDst, vk::SharingMode::eExclusive,
        0, nullptr, vk::ImageLayout::eUndefined);
    vk::Image stagingImg = ctx._device.createImage(stagingImgInfo);

    VkMemoryRequirements memRequirements = ctx._device.getImageMemoryRequirements(stagingImg);
    
    vk::MemoryAllocateInfo memAllocate(memRequirements.size);
    memAllocate.memoryTypeIndex = ctx.findMemoryType(
        memRequirements.size, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, {}, memRequirements.memoryTypeBits);
    vk::DeviceMemory stagingMemory = ctx._device.allocateMemory(memAllocate);
    ctx._device.bindImageMemory(stagingImg, stagingMemory, 0);
 
    // Copy image to staging image
    vk::CommandBufferAllocateInfo commandBufInfo(
        ctx._graphicsCommandPool, vk::CommandBufferLevel::ePrimary, 1);
    auto commandBuf = ctx._device.allocateCommandBuffers(commandBufInfo).at(0);
    commandBuf.begin(vk::CommandBufferBeginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

    insertBarrier(commandBuf, stagingImg, {}, vk::AccessFlagBits ::eTransferWrite,
                  vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
                  vk::PipelineStageFlagBits ::eTransfer,
                  vk::PipelineStageFlagBits ::eTransfer);

    vk::ImageCopy imgCopy{
        vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u),
        vk::Offset3D(),
        vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0u, 0u, 1u),
        vk::Offset3D(),
        vk::Extent3D{static_cast<u32>(_internal->_width),
            static_cast<u32>(_internal->_height), 1u}
    };
    commandBuf.copyImage(_internal->_image, vk::ImageLayout::eTransferSrcOptimal,
                                 stagingImg, vk::ImageLayout::eTransferDstOptimal, imgCopy);
    
    insertBarrier(commandBuf, stagingImg, vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits ::eMemoryRead,
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eGeneral,
        vk::PipelineStageFlagBits ::eTransfer,
        vk::PipelineStageFlagBits ::eTransfer);

    commandBuf.end();

    auto fence = ctx._device.createFence(vk::FenceCreateInfo());
    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &commandBuf);
    ctx.queue(vk::QueueFlagBits::eGraphics).submit(submitInfo, fence);

    ctx._device.waitForFences(fence, VK_TRUE, UINT64_MAX);

    // Get data from staging image
    // Get layout of the image (including row pitch)
    vk::ImageSubresource subResource{vk::ImageAspectFlagBits::eColor};
    VkSubresourceLayout subResourceLayout = ctx._device.getImageSubresourceLayout(stagingImg, subResource);

    // TODO take rowpitch into account
    const char *imageData = reinterpret_cast<char*>(ctx._device.mapMemory(stagingMemory, offset, count, {}));
    imageData += subResourceLayout.offset;
    std::memcpy(data, imageData, count);

    // Destroy resources
    ctx._device.unmapMemory(stagingMemory);
    ctx._device.freeMemory(stagingMemory);
    ctx._device.destroyImage(stagingImg);
    ctx._device.freeCommandBuffers(ctx._graphicsCommandPool, commandBuf);
    ctx._device.destroyFence(fence);
}

vk::SubresourceLayout VkwImage::getSubresourceLayout() {
    VulkanContext &ctx = Vulkan::context();
    // TODO ensure subresource is correctly selected
    vk::ImageSubresource subresource(vk::ImageAspectFlagBits::eColor, 0, 0);
    return ctx._device.getImageSubresourceLayout(_internal->_image,
                                                 subresource);
}

vk::ImageView VkwImage::getImageView() {
    if (!_internal->_imageView) {
        _internal->createImageView();
    }
    return _internal->_imageView;
}

vk::Sampler VkwImage::getSampler() {
    if (!_internal->_sampler) {
        _internal->createSampler();
    }
    return _internal->_sampler;
}

vk::Image VkwImage::handle() {
    return _internal->_image;
}

} // namespace world
