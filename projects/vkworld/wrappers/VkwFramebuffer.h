#ifndef VKWORLD_VKWFRAMEBUFFER_H
#define VKWORLD_VKWFRAMEBUFFER_H

#include "vkworld/VkWorldConfig.h"

#include "Vulkan.h"

namespace world {

class VkwFramebufferPrivate {
public:
    int _width, _height;
    vk::Format _imageFormat;
    vk::Image _image;
    vk::ImageView _imageView;
    vk::RenderPass _renderPass;
    vk::Framebuffer _framebuffer;

    VkwFramebufferPrivate(int width, int height);
    ~VkwFramebufferPrivate();

    void init();

    void createOffscreenRenderPass();
};

class VKWORLD_EXPORT VkwFramebuffer {
public:
    VkwFramebuffer(int width, int height);

    VkwFramebuffer(int width, int height, vk::Image image);

private:
    std::shared_ptr<VkwFramebufferPrivate> _internal;
};

} // namespace world

#endif // VKWORLD_VKWFRAMEBUFFER_H
