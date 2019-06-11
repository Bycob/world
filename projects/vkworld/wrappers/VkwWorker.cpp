#include "VkwWorker.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"

namespace world {

class VkwWorkerPrivate {
public:
    vk::CommandBuffer _commandBuffer;

    vk::Fence _fence;
};

VkwWorker::VkwWorker() : _internal(std::make_shared<VkwWorkerPrivate>()) {

    auto &ctx = Vulkan::context();

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        ctx._computeCommandPool, vk::CommandBufferLevel::ePrimary, 1);
    _internal->_commandBuffer =
        ctx._device.allocateCommandBuffers(commandBufferAllocateInfo)[0];

    vk::CommandBufferBeginInfo beginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    _internal->_commandBuffer.begin(beginInfo);
}

void VkwWorker::bindCommand(VkwComputePipeline &pipeline,
                            VkwDescriptorSet &dset) {
    _internal->_commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute,
                                           pipeline.getPipeline());
    _internal->_commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute, pipeline.getLayout(), 0, dset.handle(),
        std::vector<u32>());
}

void VkwWorker::dispatchCommand(u32 x, u32 y, u32 z) {
    _internal->_commandBuffer.dispatch(x, y, z);
}

void VkwWorker::endCommandRecording() { _internal->_commandBuffer.end(); }

void VkwWorker::run() {
    auto &ctx = Vulkan::context();

    _internal->_fence = ctx._device.createFence(vk::FenceCreateInfo());

    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1,
                              &_internal->_commandBuffer);
    ctx._computeQueue.submit(submitInfo, _internal->_fence);
}

void VkwWorker::waitForCompletion() {
    auto &ctx = Vulkan::context();
    ctx._device.waitForFences(_internal->_fence, true, 100000000000);
}
} // namespace world
