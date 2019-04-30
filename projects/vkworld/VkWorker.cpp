#include "VkWorker.h"

#include <vulkan/vulkan.hpp>

#include "Vulkan.h"
#include "Vulkan_p.h"

namespace world {

class VkwWorkerPrivate {
public:
    vk::CommandBuffer _commandBuffer;

    vk::Fence _fence;
};

VkwWorker::VkwWorker() : _internal(std::make_shared<VkwWorkerPrivate>()) {

    auto &vkctx = Vulkan::context().internal();

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        vkctx._computeCommandPool, vk::CommandBufferLevel::ePrimary, 1);
    _internal->_commandBuffer =
        vkctx._device.allocateCommandBuffers(commandBufferAllocateInfo)[0];

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
    auto &vkctx = Vulkan::context().internal();

    _internal->_fence = vkctx._device.createFence(vk::FenceCreateInfo());

    vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1,
                              &_internal->_commandBuffer);
    vkctx._computeQueue.submit(submitInfo, _internal->_fence);
}

void VkwWorker::waitForCompletion() {
    auto &vkctx = Vulkan::context().internal();
    vkctx._device.waitForFences(_internal->_fence, true, 100000000000);
}
} // namespace world
