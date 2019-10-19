#include "VkwGraphicsPipeline.h"

#include <array>

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"

namespace world {

class VkwGraphicsPipelinePrivate {
public:
    std::map<VkwShaderType, vk::ShaderModule> _shaders;

    vk::DescriptorSetLayout _descriptorSetLayout;

    vk::PipelineLayout _pipelineLayout;

    vk::Pipeline _pipeline;

    vk::RenderPass _renderPass;

    bool _initialized = false;

    ~VkwGraphicsPipelinePrivate() {
        if (!_initialized) {
            return;
        }

        auto &ctx = Vulkan::context();
        for (auto &entry : _shaders) {
            ctx._device.destroy(entry.second);
        }
        ctx._device.destroy(_pipelineLayout);
        ctx._device.destroy(_pipeline);
    }

    void createPipeline() {
        auto &ctx = Vulkan::context();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 1,
                                                        &_descriptorSetLayout);
        _pipelineLayout = ctx._device.createPipelineLayout(pipelineLayoutInfo);

        // Create pipeline stage info for each shader
        std::vector<vk::PipelineShaderStageCreateInfo> stageInfos;

        if (_shaders.find(VkwShaderType::VERTEX) != _shaders.end()) {
            vk::PipelineShaderStageCreateInfo vertexStageInfo(
                {}, vk::ShaderStageFlagBits::eVertex,
                _shaders[VkwShaderType::VERTEX], "main");
        }


        if (_shaders.find(VkwShaderType::FRAGMENT) != _shaders.end()) {
            vk::PipelineShaderStageCreateInfo fragmentStageInfo(
                {}, vk::ShaderStageFlagBits::eFragment,
                _shaders[VkwShaderType::FRAGMENT], "main");
        }

        // Fixed stages
        // TODO: Actually provide data if there is some
        vk::PipelineVertexInputStateCreateInfo vertInputStageInfo(
            {}, 0, nullptr, 0, nullptr);
        // TODO: Allow user to chose PrimitiveTopology
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStageInfo(
            {}, vk::PrimitiveTopology::eTriangleList);
        // TODO: Specify the size of the viewport from parameters
        vk::Viewport viewport(0, 0, 100, 100, 0, 1);
        vk::Rect2D scissor({0, 0}, {100, 100});
        vk::PipelineViewportStateCreateInfo viewportStageInfo({}, 1, &viewport,
                                                              1, &scissor);
        // TODO: Add other parameters
        vk::PipelineRasterizationStateCreateInfo rasterizationStageInfo(
            {}, false, false, vk::PolygonMode::eFill);
        // TODO: Enable multisampling
        vk::PipelineMultisampleStateCreateInfo multisampleStageInfo;

        // TODO: Add depth and stencil test

        // TODO: enable color blending if the user wants to
        vk::PipelineColorBlendAttachmentState blendAttachment(
            false, vk::BlendFactor::eOne, vk::BlendFactor::eZero,
            vk::BlendOp::eAdd, vk::BlendFactor::eOne, vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB);

        vk::PipelineColorBlendStateCreateInfo blendStageInfo(
            {}, false, vk::LogicOp::eCopy, 1, &blendAttachment);

        // Create pipeline from layout and stage info
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo({}, 2,
                                                          &stageInfos[0]);
        pipelineCreateInfo.layout = _pipelineLayout;
        pipelineCreateInfo.pVertexInputState = &vertInputStageInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStageInfo;
        pipelineCreateInfo.pViewportState = &viewportStageInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationStageInfo;
        pipelineCreateInfo.pMultisampleState = &multisampleStageInfo;
        pipelineCreateInfo.pDepthStencilState = nullptr; // Optional
        pipelineCreateInfo.pColorBlendState = &blendStageInfo;
        pipelineCreateInfo.pDynamicState = nullptr; // Optional

        pipelineCreateInfo.renderPass = _renderPass;
        pipelineCreateInfo.subpass = 0;

        // TODO: Use this when available
        pipelineCreateInfo.basePipelineHandle = nullptr;
        pipelineCreateInfo.basePipelineIndex = -1;

        // TODO: May use pipeline cache?
        _pipeline =
            ctx._device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
    }

    void initialize() {
        createPipeline();
        _initialized = true;
    }
};


VkwGraphicsPipeline::VkwGraphicsPipeline(
    VkwDescriptorSetLayout &descriptorSetLayout)
        : _internal(std::make_shared<VkwGraphicsPipelinePrivate>()) {

    _internal->_descriptorSetLayout = descriptorSetLayout.getLayout();
}

void VkwGraphicsPipeline::setBuiltinShader(VkwShaderType type,
                                           const std::string &shaderName) {
    auto &ctx = Vulkan::context();
    _internal->_shaders[type] =
        ctx.createShader(ctx.readFile(shaderName + ".spv"));
}

vk::Pipeline &VkwGraphicsPipeline::getPipeline() {
    if (!_internal->_initialized)
        _internal->initialize();
    return _internal->_pipeline;
}

vk::PipelineLayout &VkwGraphicsPipeline::getLayout() {
    if (!_internal->_initialized)
        _internal->initialize();
    return _internal->_pipelineLayout;
}
} // namespace world
