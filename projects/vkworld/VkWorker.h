#ifndef VKWORLD_VKWORKER_H
#define VKWORLD_VKWORKER_H

#include "VkWorldConfig.h"

#include <memory>

#include "DescriptorSetVk.h"
#include "ComputePipeline.h"

namespace world {

class VkWorkerPrivate;

class VKWORLD_EXPORT VkWorker {
public:
    VkWorker();

    void bindCommand(ComputePipeline &pipeline, DescriptorSetVk &dset);

    void dispatchCommand(u32 x, u32 y, u32 z);

    void endCommandRecording();

    void run();

    void waitForCompletion();

private:
    std::shared_ptr<VkWorkerPrivate> _internal;
};
} // namespace world

#endif // VKWORLD_VKWORKER_H
