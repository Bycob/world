#include "ProgramVk.h"

#include <map>

#include "Vulkan.h"
#include "BufferVk_p.h"

namespace world {

class ProgramVkPrivate {
public:
    VulkanContext &_context;

    std::map<u32, BufferVk> _buffers;

    std::map<u32, BufferVk> _uniforms;

    VkShaderModule _shader;

    u32 _dispatchX = 1, _dispatchY = 1, _dispatchZ = 1;


    bool _generated = false;

    VkDescriptorSetLayout _descriptorSetLayout;

    VkDescriptorSet _descriptorSet;

    VkPipelineLayout _pipelineLayout;

    VkPipeline _pipeline;

    VkCommandBuffer _commandBuffer;


    ProgramVkPrivate(VulkanContext &context) : _context(context) {}

    void createDescriptorSetLayout();

    void setupDescriptorSetBinding(VkDescriptorSetLayoutBinding &binding,
                                   VkDescriptorType descriptorType,
                                   u32 bindingId);

    void createDescriptorSet();

    void writeDescriptorSet(u32 binding, BufferVk &buffer,
                            VkDescriptorType descriptorType);

    void createPipelineLayout();

    void createPipeline();

    void createCommandBuffer();
};

void ProgramVkPrivate::createDescriptorSetLayout() {
    // set bindings
    u32 bindingCount = _buffers.size() + _uniforms.size();
    std::unique_ptr<VkDescriptorSetLayoutBinding[]> bindings(
        new VkDescriptorSetLayoutBinding[bindingCount]);

    u32 bindingId = 0;

    for (auto &pair : _buffers) {
        setupDescriptorSetBinding(
            bindings[bindingId], VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, pair.first);
        ++bindingId;
    }

    for (auto &pair : _uniforms) {
        setupDescriptorSetBinding(
            bindings[bindingId], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, pair.first);
        ++bindingId;
    }

    // Descriptor set
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {};
    descriptorSetLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
    descriptorSetLayoutCreateInfo.pBindings = bindings.get();

    if (vkCreateDescriptorSetLayout(_context._device,
                                    &descriptorSetLayoutCreateInfo, NULL,
                                    &_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("[Vulkan] [PProgramVk::prepare] failed create "
                                 "descriptor set layout");
    }
}

void ProgramVkPrivate::setupDescriptorSetBinding(
    VkDescriptorSetLayoutBinding &descriptorSetLayoutBinding,
    VkDescriptorType descriptorType, u32 bindingId) {
    descriptorSetLayoutBinding.binding = bindingId;
    descriptorSetLayoutBinding.descriptorType = descriptorType;
    descriptorSetLayoutBinding.descriptorCount = 1;
    descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
}

void ProgramVkPrivate::createDescriptorSet() {
    // Allocate descriptor set in the pool
    VkDescriptorSetAllocateInfo descriptorSetInfo = {};
    descriptorSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetInfo.descriptorPool = _context._descriptorPool;
    descriptorSetInfo.descriptorSetCount = 1;
    descriptorSetInfo.pSetLayouts = &_descriptorSetLayout;

    if (vkAllocateDescriptorSets(_context._device, &descriptorSetInfo,
                                 &_descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("[Vulkan] [PProgramVk::createDescriptorSet] "
                                 "failed allocate descriptor set");
    }

    // Connect buffers to descriptor
    for (auto &pair : _buffers) {
        writeDescriptorSet(pair.first, pair.second,
                           VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
    }

    for (auto &pair : _uniforms) {
        writeDescriptorSet(pair.first, pair.second,
                           VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    }
}

void world::ProgramVkPrivate::writeDescriptorSet(
    u32 binding, BufferVk &buffer, VkDescriptorType descriptorType) {
    VkDescriptorBufferInfo descriptorBufferInfo = {};
    descriptorBufferInfo.buffer = buffer.internal()._buffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = buffer.internal()._memorySize;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = _descriptorSet;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;

    vkUpdateDescriptorSets(_context._device, 1, &writeDescriptorSet, 0,
                           nullptr);
}

void ProgramVkPrivate::createPipelineLayout() {
    // Create pipeline layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;

    if (vkCreatePipelineLayout(_context._device, &pipelineLayoutInfo, nullptr,
                               &_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("[Vulkan] [PProgramVk::createPipeline] failed "
                                 "create pipeline layout");
    }
}

void ProgramVkPrivate::createPipeline() {
    // Create pipeline stage info (one stage)
    VkPipelineShaderStageCreateInfo pipelineStageInfo = {};
    pipelineStageInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipelineStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipelineStageInfo.module = _shader;
    pipelineStageInfo.pName = "main";

    // Create pipeline from layout and stage info
    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stage = pipelineStageInfo;
    pipelineCreateInfo.layout = _pipelineLayout;

    if (vkCreateComputePipelines(_context._device, VK_NULL_HANDLE, 1,
                                 &pipelineCreateInfo, nullptr,
                                 &_pipeline) != VK_SUCCESS) {
        throw std::runtime_error(
            "[Vulkan] [PProgramVk::createPipeline] failed create pipeline");
    }
}

void ProgramVkPrivate::createCommandBuffer() {
    // Allocate a command buffer
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = _context._computeCommandPool;
    commandBufferAllocateInfo.commandBufferCount = 1;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    // TODO Test VK_SUCCESS
    vkAllocateCommandBuffers(_context._device, &commandBufferAllocateInfo,
                             &_commandBuffer);


    // Writing the commands
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags =
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // the buffer is only
                                                     // submitted and used once
                                                     // in this application.

    vkBeginCommandBuffer(_commandBuffer, &beginInfo);
    vkCmdBindPipeline(_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                      _pipeline);
    vkCmdBindDescriptorSets(_commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
                            _pipelineLayout, 0, 1, &_descriptorSet, 0, NULL);

    // Dispatching the working groups
    vkCmdDispatch(_commandBuffer, _dispatchX, _dispatchY, _dispatchZ);
    // End recording
    // TODO Test VK_SUCCESS
    vkEndCommandBuffer(_commandBuffer);
}

ProgramVk::ProgramVk(VulkanContext &context)
        : _internal(std::make_shared<ProgramVkPrivate>(context)) {}

void ProgramVk::setBuffer(u32 id, BufferVk buffer) {
    _internal->_buffers.emplace(id, buffer);
}

void ProgramVk::setUniform(u32 id, BufferVk buffer) {
    _internal->_uniforms.emplace(id, buffer);
}

void ProgramVk::setEmbeddedShader(const std::string &name) {
    auto &internalCtx = _internal->_context;
    _internal->_shader =
        internalCtx.createShader(internalCtx.readFile(name + ".spv"));
}

void ProgramVk::setDispatch(u32 x, u32 y, u32 z) {
    _internal->_dispatchX = x;
    _internal->_dispatchY = y;
    _internal->_dispatchZ = z;
}

void ProgramVk::run() {
    if (!_internal->_generated) {
        _internal->createDescriptorSetLayout();
        _internal->createDescriptorSet();
        _internal->createPipelineLayout();
        _internal->createPipeline();
        _internal->createCommandBuffer();
        _internal->_generated = true;
    }

    auto &internalCtx = _internal->_context;

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1; // submit a single command buffer
    submitInfo.pCommandBuffers =
        &_internal->_commandBuffer; // the command buffer to submit.

    // Create fence
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;

    VkFence fence;
    // TODO Test VK_SUCCESS
    vkCreateFence(internalCtx._device, &fenceCreateInfo, NULL, &fence);

    // Submid queue
    // TODO Test VK_SUCCESS
    vkQueueSubmit(internalCtx._computeQueue, 1, &submitInfo, fence);
    vkWaitForFences(internalCtx._device, 1, &fence, VK_TRUE, 100000000000);
    vkDestroyFence(internalCtx._device, fence, NULL);
}

} // namespace world