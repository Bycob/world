#include "ComputePipeline.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"
#include "Vulkan_p.h"

namespace world {

class VkwComputePipelinePrivate {
public:
    vk::ShaderModule _shader;

    vk::DescriptorSetLayout _descriptorSetLayout;

    vk::PipelineLayout _pipelineLayout;

    vk::Pipeline _pipeline;

    bool _initialized = false;


    void createPipeline() {
        auto &vkctx = Vulkan::context().internal();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1,
                                                        &_descriptorSetLayout);
        _pipelineLayout =
            vkctx._device.createPipelineLayout(pipelineLayoutInfo);

        // Create pipeline stage info (one stage)
        vk::PipelineShaderStageCreateInfo pipelineStageInfo(
            {}, vk::ShaderStageFlagBits::eCompute, _shader, "main");

        // Create pipeline from layout and stage info
        vk::ComputePipelineCreateInfo pipelineCreateInfo({}, pipelineStageInfo,
                                                         _pipelineLayout);
        _pipeline = vkctx._device.createComputePipeline(vk::PipelineCache(),
                                                        pipelineCreateInfo);
    }

    void initialize() {
        createPipeline();
        _initialized = true;
    }
};


VkwComputePipeline::VkwComputePipeline(
    VkwDescriptorSetLayout &descriptorSetLayout)
        : _internal(std::make_shared<VkwComputePipelinePrivate>()) {

    _internal->_descriptorSetLayout = descriptorSetLayout.getLayout();
}

VkwComputePipeline::VkwComputePipeline(
    VkwDescriptorSetLayout &descriptorSetLayout, const std::string &shaderName)
        : VkwComputePipeline(descriptorSetLayout) {
    setBuiltinShader(shaderName);
}

void VkwComputePipeline::setBuiltinShader(const std::string &shaderName) {
    auto &internalCtx = Vulkan::context().internal();
    _internal->_shader =
        internalCtx.createShader(internalCtx.readFile(shaderName + ".spv"));
}

vk::Pipeline &VkwComputePipeline::getPipeline() {
    if (!_internal->_initialized)
        _internal->initialize();
    return _internal->_pipeline;
}

vk::PipelineLayout &VkwComputePipeline::getLayout() {
    if (!_internal->_initialized)
        _internal->initialize();
    return _internal->_pipelineLayout;
}
} // namespace world
