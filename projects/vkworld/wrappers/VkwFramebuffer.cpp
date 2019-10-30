#include "VkwFramebuffer.h"

namespace world {

VkwFramebufferPrivate::VkwFramebufferPrivate(int width, int height)
        : _width(width), _height(height) {}

void VkwFramebufferPrivate::init() {
    auto &ctx = Vulkan::context();

    // TODO: Right image format (Ctrl+F vk::Format::eUndefined)
    vk::ImageViewCreateInfo createInfo({}, _image, vk::ImageViewType::e2D,
                                       _imageFormat);
    createInfo.components.r = createInfo.components.g =
        createInfo.components.b = createInfo.components.a =
            vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    _imageView = ctx._device.createImageView(createInfo);

    // TODO: Get image attachments from the swap chain or from an offscreen
    // rendering
    vk::FramebufferCreateInfo framebufferInfo({}, _renderPass, 1, &_imageView,
                                              _width, _height, 1);
    _framebuffer = ctx._device.createFramebuffer(framebufferInfo);
}

VkwFramebufferPrivate::~VkwFramebufferPrivate() {
    auto &ctx = Vulkan::context();
    ctx._device.destroy(_framebuffer);
    ctx._device.destroy(_renderPass);
}

// Offscreen render
// https://github.com/SaschaWillems/Vulkan/blob/master/examples/offscreen/offscreen.cpp

void VkwFramebufferPrivate::createOffscreenRenderPass() {
    auto &ctx = Vulkan::context();

    _imageFormat = vk::Format::eUndefined;

    // Create the image and image view
    // TODO: Check what ImageUsage really means + use correct format
    vk::ImageCreateInfo imageInfo(
        {}, vk::ImageType::e2D, _imageFormat,
        {static_cast<u32>(_width), static_cast<u32>(_height), 1}, 1, 1,
        vk::SampleCountFlagBits::e1, vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eColorAttachment |
            vk::ImageUsageFlagBits::eSampled);
    _image = ctx._device.createImage(imageInfo);

    // TODO: Allocate image, bind memory. Use the memory cache.

    // Sampler
    vk::SamplerCreateInfo samplerInfo(
        {}, vk::Filter::eLinear, vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear, vk::SamplerAddressMode::eClampToEdge,
        vk::SamplerAddressMode::eClampToEdge,
        vk::SamplerAddressMode::eClampToEdge, 0.0f, VK_FALSE, 1.0f, VK_FALSE,
        vk::CompareOp::eAlways, 0.0f, 1.0f, vk::BorderColor::eFloatOpaqueWhite);
    vk::Sampler sampler = ctx._device.createSampler(samplerInfo);
}

VkwFramebuffer::VkwFramebuffer(int width, int height)
        : _internal(std::make_shared<VkwFramebufferPrivate>(width, height)) {
    _internal->createOffscreenRenderPass();
    _internal->init();
}

VkwFramebuffer::VkwFramebuffer(int width, int height, vk::Image image)
        : VkwFramebuffer(width, height) {
    _internal->_image = image;
    _internal->init();
}

} // namespace world
