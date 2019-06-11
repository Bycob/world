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
    VkwSubBuffer _params;
    VkwSubBuffer _positions;
    VkwSubBuffer _vertices;
    VkwSubBuffer _faces;

    std::unique_ptr<VkwWorker> _worker;
};

struct BushesParams {};

Bushes::Bushes() : _internal(new BushesPrivate()) {}

Bushes::~Bushes() { delete _internal; }

void Bushes::addPosition(const vec3d &position) {
    _positions.push_back(position);
}

void Bushes::collect(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) {
    prepare();
    updateData();
    _internal->_worker->run();
    flush(collector);
}

void Bushes::prepare() {
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
    _internal->_vertices = vkctx.allocate(
        verticesSize, DescriptorType::STORAGE_BUFFER, MemoryType::CPU_READS);

    // 2 triangle faces per "face"
    u32 facesSize = faceCount * 6 * sizeof(float);
    _internal->_faces = vkctx.allocate(
        facesSize, DescriptorType::STORAGE_BUFFER, MemoryType::CPU_READS);

    // Input
    u32 positionsSize = _positions.size() * 3 * sizeof(float);
    _internal->_positions = vkctx.allocate(
        positionsSize, DescriptorType::STORAGE_BUFFER, MemoryType::CPU_WRITES);

    _internal->_params =
        vkctx.allocate(sizeof(BushesParams), DescriptorType::UNIFORM_BUFFER,
                       MemoryType::CPU_WRITES);

    VkwDescriptorSet dset(layout);
    dset.addDescriptor(0, _internal->_params);
    dset.addDescriptor(1, _internal->_positions);
    dset.addDescriptor(2, _internal->_vertices);
    dset.addDescriptor(3, _internal->_faces);
    // dset.addDescriptor(255, DescriptorType::STORAGE_BUFFER, );

    _internal->_worker = std::make_unique<VkwWorker>();
    _internal->_worker->bindCommand(pipeline, dset);
    _internal->_worker->dispatchCommand(xGroupCount, yGroupCount, zGroupCount);

    _internal->_worker->endCommandRecording();
}

void Bushes::updateData() {
    std::vector<float> positions;
    for (const auto &pos : _positions) {
        positions.push_back(pos.x);
        positions.push_back(pos.y);
        positions.push_back(pos.z);
    }
    _internal->_positions.setData(&positions[0]);

    BushesParams paramsStruct{};
    _internal->_params.setData(&paramsStruct);
}

void Bushes::flush(ICollector &collector) {
    _internal->_worker->waitForCompletion();

    // Collect
    VkwMemoryHelper::GPUToMesh(_internal->_vertices, _internal->_faces, _mesh);

    // Destroy
    // TODO
}

} // namespace world
