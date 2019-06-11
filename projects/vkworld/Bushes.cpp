#include "Bushes.h"

#include "Vulkan_p.h"
#include "DescriptorSetLayoutVk.h"
#include "ComputePipeline.h"
#include "DescriptorSetVk.h"
#include "VkWorker.h"
#include "VkwMemoryHelper.h"

namespace world {

class BushesPrivate {
public:
};

Bushes::Bushes() : _internal(new BushesPrivate()) {}

Bushes::~Bushes() { delete _internal; }

void Bushes::addPosition(const vec3d &position) {
    _positions.push_back(position);
}

void Bushes::collect(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) {

    auto &vkctx = Vulkan::context().internal();
    VkwDescriptorSetLayout layout({0}, {1, 2, 3, 255});
    VkwComputePipeline pipeline(layout, "bushes");

    const u32 xGroupSize = 32;
    const u32 yGroupSize = 8;
    const u32 zGroupSize = 4;

    const u32 xGroupCount = (_positions.size() - 1) / xGroupSize + 1;
    const u32 yGroupCount = (_count - 1) / yGroupSize + 1;
    const u32 zGroupCount = (_bladeSegments - 1) / zGroupSize + 1;

    // Output
    u32 faceCount = _positions.size() * _count * _bladeSegments;
    // 2 vertices per face + 2, 8 float per vertex
    u32 verticesSize = (faceCount + 1) * 2 * 8 * sizeof(float);
    auto verticesBuffer = vkctx.allocate(
        verticesSize, DescriptorType::STORAGE_BUFFER, MemoryType::CPU_READS);

    // 2 triangle faces per "face"
    u32 facesSize = faceCount * 6 * sizeof(float);
    auto facesBuffer = vkctx.allocate(facesSize, DescriptorType::STORAGE_BUFFER,
                                      MemoryType::CPU_READS);

    // Input
    u32 positionsSize = _positions.size() * 3 * sizeof(float);
    auto positionsBuffer = vkctx.allocate(
        positionsSize, DescriptorType::STORAGE_BUFFER, MemoryType::CPU_WRITES);

    std::vector<float> positions;
    for (const auto &pos : _positions) {
        positions.push_back(pos.x);
        positions.push_back(pos.y);
        positions.push_back(pos.z);
    }
    positionsBuffer.setData(&positions[0]);

    struct {

    } paramsStruct;

    auto paramsBuffer =
        vkctx.allocate(sizeof(paramsStruct), DescriptorType::UNIFORM_BUFFER,
                       MemoryType::CPU_WRITES);
    paramsBuffer.setData(&paramsStruct);

    VkwDescriptorSet dset(layout);
    dset.addDescriptor(0, DescriptorType::UNIFORM_BUFFER, paramsBuffer);
    dset.addDescriptor(1, DescriptorType::STORAGE_BUFFER, positionsBuffer);
    dset.addDescriptor(2, DescriptorType::STORAGE_BUFFER, verticesBuffer);
    dset.addDescriptor(3, DescriptorType::STORAGE_BUFFER, facesBuffer);
    // dset.addDescriptor(255, DescriptorType::STORAGE_BUFFER, );

    VkwWorker worker;
    worker.bindCommand(pipeline, dset);
    worker.dispatchCommand(xGroupCount, yGroupCount, zGroupCount);

    worker.endCommandRecording();
    worker.run();

    worker.waitForCompletion();

    // Collect
    VkwMemoryHelper::GPUToMesh(verticesBuffer, facesBuffer, _mesh);

    // Destroy
}

void Bushes::prepare() {}

void Bushes::run() {}

void Bushes::flush(ICollector &collector) {}

} // namespace world
