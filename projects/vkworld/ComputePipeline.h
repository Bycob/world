#ifndef VKWORLD_COMPUTEPIPELINE_H
#define VKWORLD_COMPUTEPIPELINE_H

#include "VkWorldConfig.h"

#include <memory>

#include <world/core/WorldTypes.h>

#include "VkEnums.h"
#include "DescriptorSetLayoutVk.h"

namespace vk {
class Pipeline;
class PipelineLayout;
} // namespace vk

namespace world {

class ComputePipelinePrivate;

class VKWORLD_EXPORT ComputePipeline {
public:
    ComputePipeline(DescriptorSetLayoutVk &descriptorSetLayout);

    ComputePipeline(DescriptorSetLayoutVk &descriptorSetLayout,
                    const std::string &shaderName);

    void setBuiltinShader(const std::string &shaderName);

    /** Returns the pipeline created with current configuration of this
     * ComputePipeline. */
    vk::Pipeline &getPipeline();

    vk::PipelineLayout &getLayout();

private:
    std::shared_ptr<ComputePipelinePrivate> _internal;
};
} // namespace world

#endif // VKWORLD_COMPUTEPIPELINE_H
