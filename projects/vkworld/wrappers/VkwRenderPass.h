#ifndef VKWORLD_VKWFRAMEBUFFER_H
#define VKWORLD_VKWFRAMEBUFFER_H

#include "vkworld/VkWorldConfig.h"

#include "Vulkan.h"
#include "VkwImage.h"

namespace world {

class VkwRenderPassPrivate;

class VKWORLD_EXPORT VkwRenderPass {
public:
    VkwRenderPass(const VkwImage &image);

    VkwRenderPass(int width, int height);

    vk::RenderPass handle();

    vk::Framebuffer framebuffer();

    VkwImage image();

private:
    std::shared_ptr<VkwRenderPassPrivate> _internal;
};

} // namespace world

#endif // VKWORLD_VKWFRAMEBUFFER_H
