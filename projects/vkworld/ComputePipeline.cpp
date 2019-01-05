#include "ComputePipeline.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"
#include "Vulkan_p.h"

namespace world {

class ComputePipelinePrivate {
public:
    vk::ShaderModule _shader;

    vk::DescriptorSetLayout _descriptorSetLayout;

    vk::PipelineLayout _pipelineLayout;

    vk::Pipeline _pipeline;

    bool _initialized = false;
    ;


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


ComputePipeline::ComputePipeline(DescriptorSetLayoutVk &descriptorSetLayout)
        : _internal(std::make_shared<ComputePipelinePrivate>()) {

    _internal->_descriptorSetLayout = descriptorSetLayout.getLayout();
}

ComputePipeline::ComputePipeline(DescriptorSetLayoutVk &descriptorSetLayout,
                                 const std::string &shaderName)
        : ComputePipeline(descriptorSetLayout) {
    setBuiltinShader(shaderName);
}

void ComputePipeline::setBuiltinShader(const std::string &shaderName) {
    auto &internalCtx = Vulkan::context().internal();
    _internal->_shader =
        internalCtx.createShader(internalCtx.readFile(shaderName + ".spv"));
}

vk::Pipeline &ComputePipeline::getPipeline() {
    if (!_internal->_initialized)
        _internal->initialize();
    return _internal->_pipeline;
}

vk::PipelineLayout &ComputePipeline::getLayout() {
    if (!_internal->_initialized)
        _internal->initialize();
    return _internal->_pipelineLayout;
}
} // namespace world
