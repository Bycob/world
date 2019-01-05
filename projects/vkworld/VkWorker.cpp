#include "VkWorker.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"
#include "Vulkan_p.h"

namespace world {

class VkWorkerPrivate {
public:
    vk::CommandBuffer _commandBuffer;

    vk::Fence _fence;
};

VkWorker::VkWorker() : _internal(std::make_shared<VkWorkerPrivate>()) {

    auto &vkctx = Vulkan::context().internal();

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        vkctx._computeCommandPool, vk::CommandBufferLevel::ePrimary, 1);
    _internal->_commandBuffer =
        vkctx._device.allocateCommandBuffers(commandBufferAllocateInfo)[0];

    vk::CommandBufferBeginInfo beginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    _internal->_commandBuffer.begin(beginInfo);
}

void VkWorker::bindCommand(ComputePipeline &pipeline, DescriptorSetVk &dset) {
    _internal->_commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute,
                                           pipeline.getPipeline());
    _internal->_commandBuffer.bindDescriptorSets(
        vk::PipelineBindPoint::eCompute, pipeline.getLayout(), 0, dset.handle(),
        std::vector<u32>());
}

void VkWorker::dispatchCommand(u32 x, u32 y, u32 z) {
    _internal->_commandBuffer.dispatch(x, y, z);
}

void VkWorker::endCommandRecording() { _internal->_commandBuffer.end(); }

void VkWorker::run() {
    auto &vkctx = Vulkan::context().internal();

    _internal->_fence = vkctx._device.createFence(vk::FenceCreateInfo());

    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1,
                              &_internal->_commandBuffer);
    vkctx._computeQueue.submit(submitInfo, _internal->_fence);
}

void VkWorker::waitForCompletion() {
    auto &vkctx = Vulkan::context().internal();
    vkctx._device.waitForFences(_internal->_fence, true, 100000000000);
}
} // namespace world
