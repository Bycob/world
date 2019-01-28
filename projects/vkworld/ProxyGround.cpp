#include "ProxyGround.h"
#include "ProxyGround_p.h"

#include <set>

#include <world/assets/Image.h>
#include <world/math/Perlin.h>
#include <world/core/Profiler.h>

#include "Vulkan_p.h"
#include "DescriptorSetVk.h"
#include "ComputePipeline.h"

namespace world {

ProxyGround::ProxyGround(f64 width, u32 resolution) {
    const u32 tileSize = 128;
    const u32 tileCount = resolution / tileSize;
    _internal = new ProxyGroundPrivate(width / tileCount, tileSize, tileCount);
}

ProxyGround::~ProxyGround() { delete _internal; }

void ProxyGround::collect(ICollector &collector,
                          const IResolutionModel &resolutionModel) {
    // Init collector
    if (!collector.hasChannel<Image>())
        return;

    auto &imageChan = collector.getChannel<Image>();

    // Create pipeline for vulkan processing
    auto &vkctx = Vulkan::context().internal();

    DescriptorSetLayoutVk layout1;
    layout1.addBinding(DescriptorType::UNIFORM_BUFFER, 0);
    layout1.addBinding(DescriptorType::STORAGE_BUFFER, 1);

    ComputePipeline pipeline1(layout1, "noise-random");

    const u32 workgroupSize = 32;


    // Get coordinates of processed tiles
    auto &tileSystem = _internal->_tileSystem;
    u32 bufferCount = tileSystem._bufferRes.x * tileSystem._bufferRes.y;
    u32 bufferSize = bufferCount * sizeof(float);

    BoundingBox &bbox = _internal->_bbox;
    std::set<TileCoordinates> coords;

    for (auto it = _internal->_tileSystem.iterate(resolutionModel, bbox);
         !it.endReached(); ++it) {
        auto &tileCoords = *it;
        coords.insert(tileCoords);
    }

    std::set<TileCoordinates> toAdd;
    // ...

    // Perlin parameters (TEMP)
    Perlin perlin;
    auto hash = perlin.getHash();

    const u32 img_width = tileSystem._bufferRes.x;
    const u32 img_height = tileSystem._bufferRes.y;

    struct {
        u32 width;
        u32 height;
        u32 depth = 1;
    } s_outputData;

    s_outputData.width = img_width;
    s_outputData.height = img_height;

    struct {
        u32 octaves = 12;
        u32 octaveRef = 0;
        s32 offsetX = 1;
        s32 offsetY = 0;
        s32 offsetZ = 0;
        float frequence = 8;
        float persistence = 0.4;
    } s_perlinData;

    VkSubBuffer hashBuf = vkctx.allocate(
        hash.size(), DescriptorType::UNIFORM_BUFFER, MemoryType::CPU_WRITES);
    // VkSubBuffer outputDataBuf = vkctx.allocate(sizeof(s_outputData),
    // DescriptorType::UNIFORM_BUFFER, MemoryType::CPU_WRITES); VkSubBuffer
    // perlinDataBuf = vkctx.allocate(sizeof(s_perlinData),
    // DescriptorType::UNIFORM_BUFFER, MemoryType::CPU_WRITES);

    Profiler profiler;
    std::cout << "starting" << std::endl;
    profiler.endStartSection("calculus");
    // Process tiles
    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);

        tileData._vkData = std::make_unique<ProxyGroundDataPrivate::VkData>(
            ProxyGroundDataPrivate::VkData{
                vkctx.allocate(bufferSize * 4, DescriptorType::STORAGE_BUFFER,
                               MemoryType::CPU_READS),
                vkctx.allocate(bufferSize, DescriptorType::STORAGE_BUFFER,
                               MemoryType::CPU_READS),
                VkWorker()});

        DescriptorSetVk dset(layout1);
        dset.addDescriptor(0, DescriptorType::UNIFORM_BUFFER, hashBuf);
        // dset.addDescriptor(1, DescriptorType::UNIFORM_BUFFER, outputDataBuf);
        // dset.addDescriptor(2, DescriptorType::UNIFORM_BUFFER, perlinDataBuf);
        dset.addDescriptor(1, DescriptorType::STORAGE_BUFFER,
                           tileData._vkData->_height);

        VkWorker &worker = tileData._vkData->_worker;
        worker.bindCommand(pipeline1, dset);
        worker.dispatchCommand(bufferCount / workgroupSize,
                               bufferCount / workgroupSize, 1);
        worker.endCommandRecording();

        worker.run();
    }
    std::cout << "calculating" << std::endl;

    // Collecting images
    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);
        tileData._vkData->_worker.waitForCompletion();
        std::cout << tc._pos << " completed !" << std::endl;
    }
    profiler.endStartSection("collect");
    profiler.dump();

    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);

        float *buffer = new float[bufferCount * 4];
        tileData._vkData->_height.getData(buffer);
        Image img(tileSystem._bufferRes.x, tileSystem._bufferRes.y,
                  ImageType::RGBA);

        for (u32 y = 0; y < img.height(); ++y) {
            for (u32 x = 0; x < img.width(); ++x) {
                u32 pos = (y * img.width() + x) * 4;
                img.rgba(x, y).setf(buffer[pos], buffer[pos + 1],
                                    buffer[pos + 2], buffer[pos + 3]);
            }
        }

        ItemKey key = ItemKeys::inObject(
            tc._pos.x + 0xFFFF * tc._pos.y); // TODO better id ?
                                             // _tileSystem.getId() ?
        imageChan.put(key, img);
        std::cout << "saved image at " << tc._pos << std::endl;
    }

    profiler.endSection();
    profiler.dump();
}

ProxyGroundDataPrivate &ProxyGround::getData(
    const TileCoordinates &tileCoords) {
    auto &found = _internal->_data.find(tileCoords);
    if (found == _internal->_data.end()) {
        return *(_internal->_data[tileCoords] =
                     std::make_unique<ProxyGroundDataPrivate>());
    } else {
        return *found->second;
    }
}

} // namespace world
