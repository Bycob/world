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
    colorAttachment.finalLayout = vk::ImageLayout::eTransferSrcOptimal;

    vk::AttachmentReference attachmentRef(
        0, vk::ImageLayout::eColorAttachmentOptimal);
    vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0,
                                   nullptr, 1, &attachmentRef);

    // Subpass dependencies (for layout transitions)
    std::array<vk::SubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    dependencies[0].dstStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies[0].srcAccessMask = vk::AccessFlagBits::eMemoryRead;
    dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                    vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
    dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentRead |
                                    vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies[1].dstAccessMask = vk::AccessFlagBits::eMemoryRead;
    dependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    vk::RenderPassCreateInfo renderPassInfo(
        {}, 1, &colorAttachment, 1, &subpass,
        static_cast<uint32_t>(dependencies.size()), dependencies.data());
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
