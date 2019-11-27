#include "VkwRenderPass.h"

namespace world {

class VkwRenderPassPrivate {
public:
    vk::RenderPass _renderPass;
    vk::Framebuffer _framebuffer;

    VkwImage _dstImage;

    VkwRenderPassPrivate(const VkwImage &image) : _dstImage(image) {}

    ~VkwRenderPassPrivate();

    void init();
};

void VkwRenderPassPrivate::init() {
    auto &ctx = Vulkan::context();

    // --- Create Render Pass
    vk::AttachmentDescription colorAttachment({}, _dstImage.format());
    colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
    // TODO: ePresentSrcKHR if we need to display it to the presentation
    colorAttachment.finalLayout = vk::ImageLayout::eGeneral;

    vk::AttachmentReference attachmentRef(
        0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0,
                                   nullptr, 1, &attachmentRef);

    vk::RenderPassCreateInfo renderPassInfo({}, 1, &colorAttachment, 1,
                                            &subpass);
    _renderPass = ctx._device.createRenderPass(renderPassInfo);

    // --- Framebuffer
    vk::ImageView imgView = _dstImage.getImageView();
    vk::FramebufferCreateInfo framebufferInfo(
        {}, _renderPass, 1, &imgView, _dstImage.width(), _dstImage.height(), 1);
    _framebuffer = ctx._device.createFramebuffer(framebufferInfo);
}

VkwRenderPassPrivate::~VkwRenderPassPrivate() {
    auto &ctx = Vulkan::context();
    ctx._device.destroy(_framebuffer);
    ctx._device.destroy(_renderPass);
}

// Offscreen render
// https://github.com/SaschaWillems/Vulkan/blob/master/examples/offscreen/offscreen.cpp

VkwRenderPass::VkwRenderPass(int width, int height)
        : _internal(
              std::make_shared<VkwRenderPassPrivate>(VkwImage(width, height))) {
    // TODO ensure image has correct format
    _internal->init();
}

VkwRenderPass::VkwRenderPass(const VkwImage &image)
        : _internal(std::make_shared<VkwRenderPassPrivate>(image)) {
    _internal->init();
}

vk::RenderPass VkwRenderPass::handle() { return _internal->_renderPass; }

vk::Framebuffer VkwRenderPass::framebuffer() { return _internal->_framebuffer; }

VkwImage VkwRenderPass::image() { return _internal->_dstImage; }

} // namespace world
