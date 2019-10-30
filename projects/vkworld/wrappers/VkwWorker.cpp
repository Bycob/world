#include "VkwWorker.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"

namespace world {

VkwWorker::VkwWorker() {}

VkwWorker::~VkwWorker() {
    auto &ctx = Vulkan::context();

    if (_fence) {
        ctx._device.destroy(_fence);
    }
}

void VkwWorker::endCommandRecording() { _commandBuffer.end(); }

void VkwWorker::waitForCompletion() {
    auto &ctx = Vulkan::context();
    ctx._device.waitForFences(_fence, true, 100000000000);
}


// ===== VkwComputeWorker =====


VkwComputeWorker::VkwComputeWorker() {
    auto &ctx = Vulkan::context();

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        ctx._computeCommandPool, vk::CommandBufferLevel::ePrimary, 1);
    _commandBuffer =
        ctx._device.allocateCommandBuffers(commandBufferAllocateInfo)[0];

    vk::CommandBufferBeginInfo beginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    _commandBuffer.begin(beginInfo);
}

VkwComputeWorker::~VkwComputeWorker() {
    auto &ctx = Vulkan::context();
    ctx._device.free(ctx._computeCommandPool, {_commandBuffer});
}

void VkwComputeWorker::bindCommand(VkwComputePipeline &pipeline,
                                   VkwDescriptorSet &dset) {
    _commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute,
                                pipeline.getPipeline());
    _commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute,
                                      pipeline.getLayout(), 0, dset.handle(),
                                      std::vector<u32>());

    _boundPipelines.push_back(pipeline);
    _boundDsets.push_back(dset);
}

void VkwComputeWorker::dispatchCommand(u32 x, u32 y, u32 z) {
    _commandBuffer.dispatch(x, y, z);
}

void VkwComputeWorker::run() {
    auto &ctx = Vulkan::context();

    _fence = ctx._device.createFence(vk::FenceCreateInfo());

    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &_commandBuffer);
    ctx.queue(vk::QueueFlagBits::eCompute).submit(submitInfo, _fence);
}


// ===== VkwGraphicsWorker =====


VkwGraphicsWorker::VkwGraphicsWorker() {
    auto &ctx = Vulkan::context();

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        ctx._graphicsCommandPool, vk::CommandBufferLevel::ePrimary, 1);
    _commandBuffer =
        ctx._device.allocateCommandBuffers(commandBufferAllocateInfo)[0];
}

VkwGraphicsWorker::~VkwGraphicsWorker() {
    auto &ctx = Vulkan::context();
    ctx._device.free(ctx._graphicsCommandPool, {_commandBuffer});
}

void VkwGraphicsWorker::bindCommand(VkwGraphicsPipeline &pipeline,
                                    VkwDescriptorSet &dset) {
    _commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics,
                                pipeline.getPipeline());
    _commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                      pipeline.getLayout(), 0, dset.handle(),
                                      std::vector<u32>());

    _boundPipelines.push_back(pipeline);
    _boundDsets.push_back(dset);
}

void VkwGraphicsWorker::beginRenderPass(vk::RenderPass renderPass,
                                        vk::Framebuffer framebuffer, int width,
                                        int height) {
    vk::ClearValue clearValues[2];
    clearValues[0].color = std::array<float, 4>{0.0f, 0.0f, 0.0f, 0.0f};
    clearValues[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

    // Set viewport
    // Set scissors
    vk::Rect2D renderArea(vk::Offset2D(), vk::Extent2D(width, height));

    vk::RenderPassBeginInfo renderPassBeginInfo(_renderPass, framebuffer,
                                                renderArea, 2, clearValues);
    _commandBuffer.beginRenderPass(renderPassBeginInfo,
                                   vk::SubpassContents::eInline);
}

void VkwGraphicsWorker::draw(int count) {
    _commandBuffer.drawIndexed(count, 1, 0, 0, 0);
}

void VkwGraphicsWorker::endRenderPass() { _commandBuffer.endRenderPass(); }

void VkwGraphicsWorker::run() {
    auto &ctx = Vulkan::context();

    _fence = ctx._device.createFence(vk::FenceCreateInfo());

    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &_commandBuffer);
    ctx.queue(vk::QueueFlagBits::eGraphics).submit(submitInfo, _fence);
}

} // namespace world
