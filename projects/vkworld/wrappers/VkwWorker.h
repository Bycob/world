#ifndef VKWORLD_VKWORKER_H
#define VKWORLD_VKWORKER_H

#include "VkWorldConfig.h"

#include <memory>

#include "VkwDescriptorSet.h"
#include "VkwComputePipeline.h"

namespace world {

class VkwWorkerPrivate;

class VKWORLD_EXPORT VkwWorker {
public:
    VkwWorker();

    void bindCommand(VkwComputePipeline &pipeline, VkwDescriptorSet &dset);

    void dispatchCommand(u32 x, u32 y, u32 z);

    void endCommandRecording();

    void run();

    void waitForCompletion();

private:
    std::shared_ptr<VkwWorkerPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_VKWORKER_H
