#ifndef VKWORLD_VKWORKER_H
#define VKWORLD_VKWORKER_H

#include "vkworld/VkWorldConfig.h"

#include <memory>

#include "VkwSubBuffer.h"
#include "VkwDescriptorSet.h"
#include "VkwComputePipeline.h"
#include "VkwGraphicsPipeline.h"
#include "VkwRenderPass.h"

namespace world {

class VKWORLD_EXPORT VkwWorker {
public:
    VkwWorker();

    virtual ~VkwWorker();

    VkwWorker(const VkwWorker &other) = delete;

    VkwWorker &operator=(const VkwWorker &other) = delete;

    void endCommandRecording();

    // Execution control
    virtual void run() = 0;

    void waitForCompletion();

protected:
    vk::CommandBuffer _commandBuffer;

    vk::Fence _fence;

    std::vector<VkwDescriptorSet> _boundDsets;
};

class VKWORLD_EXPORT VkwComputeWorker : public VkwWorker {
public:
    VkwComputeWorker();

    ~VkwComputeWorker() override;

    void bindCommand(VkwComputePipeline &pipeline, VkwDescriptorSet &dset);

    void dispatchCommand(u32 x, u32 y, u32 z);

    void run() override;

private:
    std::vector<VkwComputePipeline> _boundPipelines;
};

class VKWORLD_EXPORT VkwGraphicsWorker : public VkwWorker {
public:
    VkwGraphicsWorker();

    ~VkwGraphicsWorker() override;

    void beginRenderPass(VkwRenderPass &renderPass);

    void bindCommand(VkwGraphicsPipeline &pipeline, VkwDescriptorSet &dset);

    void draw(u32 count);

    // TODO keep ref of all resources so that we dont have to bother
    void drawIndexed(VkwSubBuffer &indicesBuf, VkwSubBuffer &verticesBuf,
                     u32 count);

    void endRenderPass();

    void run() override;

private:
    std::vector<VkwGraphicsPipeline> _boundPipelines;
    std::vector<VkwRenderPass> _renderPasses;
};
} // namespace world

#endif // VKWORLD_VKWORKER_H
