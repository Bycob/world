#include "VkwComputePipeline.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"

namespace world {

class VkwComputePipelinePrivate {
public:
    vk::ShaderModule _shader;

    vk::DescriptorSetLayout _descriptorSetLayout;

    vk::PipelineLayout _pipelineLayout;

    vk::Pipeline _pipeline;

    bool _initialized = false;

    ~VkwComputePipelinePrivate() {
        if (!_initialized) {
            return;
        }

        auto &ctx = Vulkan::context();
        ctx._device.destroy(_shader);
        ctx._device.destroy(_pipelineLayout);
        ctx._device.destroy(_pipeline);
    }


    void createPipeline() {
        auto &ctx = Vulkan::context();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1,
                                                        &_descriptorSetLayout);
        _pipelineLayout = ctx._device.createPipelineLayout(pipelineLayoutInfo);

        // Create pipeline stage info (one stage)
        vk::PipelineShaderStageCreateInfo pipelineStageInfo(
            {}, vk::ShaderStageFlagBits::eCompute, _shader, "main");

        // Create pipeline from layout and stage info
        vk::ComputePipelineCreateInfo pipelineCreateInfo({}, pipelineStageInfo,
                                                         _pipelineLayout);
        _pipeline = ctx._device.createComputePipeline(vk::PipelineCache(),
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
    auto &ctx = Vulkan::context();
    _internal->_shader = ctx.createShader(ctx.readFile(shaderName + ".spv"));
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
