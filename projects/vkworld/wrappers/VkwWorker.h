#ifndef VKWORLD_VKWORKER_H
#define VKWORLD_VKWORKER_H

#include "vkworld/VkWorldConfig.h"

#include <memory>

#include "VkwDescriptorSet.h"
#include "VkwComputePipeline.h"

namespace world {

class VkwWorkerPrivate;

class VKWORLD_EXPORT VkwWorker {
public:
    VkwWorker();

    ~VkwWorker();

    VkwWorker(const VkwWorker &other) = delete;

    VkwWorker &operator=(const VkwWorker &other) = delete;

    void bindCommand(VkwComputePipeline &pipeline, VkwDescriptorSet &dset);

    void dispatchCommand(u32 x, u32 y, u32 z);

    void endCommandRecording();

    void run();

    void waitForCompletion();

private:
    vk::CommandBuffer _commandBuffer;

    vk::Fence _fence;

    std::vector<VkwComputePipeline> _boundPipelines;

    std::vector<VkwDescriptorSet> _boundDsets;
};
} // namespace world

#endif // VKWORLD_VKWORKER_H
