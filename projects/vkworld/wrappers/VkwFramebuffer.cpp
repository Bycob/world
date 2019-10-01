#include "VkwFramebuffer.h"

namespace world {

VkwFramebufferPrivate::VkwFramebufferPrivate(int width, int height)
        : _width(width), _height(height) {

    auto &ctx = Vulkan::context();

    // TODO: Right image format (Ctrl+F vk::Format::eUndefined)
    vk::ImageViewCreateInfo createInfo({}, _image, vk::ImageViewType::e2D,
                                       vk::Format::eUndefined);
    createInfo.components.r = createInfo.components.g =
        createInfo.components.b = createInfo.components.a =
            vk::ComponentSwizzle::eIdentity;
    createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    createInfo.subresourceRange.baseMipLevel = 0;
    createInfo.subresourceRange.levelCount = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount = 1;

    vk::ImageView attachment = ctx._device.createImageView(createInfo);

    // TODO: Get image attachments from the swap chain or from an offscreen
    // rendering
    vk::FramebufferCreateInfo framebufferInfo({}, _renderPass, 1, &attachment,
                                              width, height, 1);
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

    // Create the image and image view
    // TODO: Check what ImageUsage really means
    vk::ImageCreateInfo imageInfo(
        {}, vk::ImageType::e2D, vk::Format::eUndefined,
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

    // Create Render Pass
    // TODO: Setup format for offline rendering
    vk::AttachmentDescription colorAttachment({}, vk::Format::eUndefined);
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    // TODO: ePresentSrcKHR if we need to display it
    colorAttachment.finalLayout = vk::ImageLayout::eTransferDstOptimal;

    vk::AttachmentReference attachmentRef(
            0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0,
                                   nullptr, 1, &attachmentRef);

    vk::RenderPassCreateInfo renderPassInfo({}, 1, &colorAttachment, 1,
                                            &subpass);
    _renderPass = ctx._device.createRenderPass(renderPassInfo);
}

VkwFramebuffer::VkwFramebuffer(int width, int height)
        : _internal(std::make_shared<VkwFramebufferPrivate>(width, height)) {}

} // namespace world
