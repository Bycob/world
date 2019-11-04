#ifndef VKWORLD_GRAPHICSPIPELINE_H
#define VKWORLD_GRAPHICSPIPELINE_H

#include "vkworld/VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include <vulkan/vulkan.hpp>

#include "VkwEnums.h"
#include "VkwDescriptorSetLayout.h"

namespace world {

class VkwGraphicsPipelinePrivate;

enum class VkwShaderType {
    VERTEX,
    TESSELATION,
    GEOMETRY,
    FRAGMENT,
    COMPUTE,
};

class VKWORLD_EXPORT VkwGraphicsPipeline {
public:
    VkwGraphicsPipeline(VkwDescriptorSetLayout &descriptorSetLayout);

    /** Enable or disable vertex buffer. Vertex buffer is enabled by default. */
    void enableVertexBuffer(bool enabled);

    void setDimensions(u32 width, u32 height);

    void setRenderPass(vk::RenderPass renderPass);

    void setBuiltinShader(VkwShaderType type, const std::string &shaderName);

    /** Returns the pipeline created with current configuration of this
     * GraphicsPipeline. */
    vk::Pipeline &getPipeline();

    vk::PipelineLayout &getLayout();

private:
    std::shared_ptr<VkwGraphicsPipelinePrivate> _internal;
};
} // namespace world

#endif // VKWORLD_GRAPHICSPIPELINE_H
