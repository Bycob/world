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
    layout1.addBinding(DescriptorType::UNIFORM_BUFFER, 1);
    layout1.addBinding(DescriptorType::STORAGE_BUFFER, 2);
    layout1.addBinding(DescriptorType::STORAGE_BUFFER, 3);

    ComputePipeline pipeline1(layout1, "noise-perlin");

    const u32 workgroupSize = 32;


    // Get coordinates of processed tiles
    auto &tileSystem = _internal->_tileSystem;
    const u32 bufferWidth = tileSystem._bufferRes.x;
    const u32 bufferHeight = tileSystem._bufferRes.y;
    const u32 bufferCount = bufferWidth * bufferHeight;
    const u32 bufferSize = bufferCount * sizeof(float);

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
    std::vector<u32> hash;

    for (auto i : perlin.getHash()) {
        hash.push_back(static_cast<u32>(i));
    }

    struct {
        u32 width;
        u32 height;
        u32 depth;
    } s_outputData;

    s_outputData.width = bufferWidth;
    s_outputData.height = bufferHeight;
    s_outputData.depth = 1;

    struct {
        u32 octaves = 12;
        u32 octaveRef = 2;
        s32 offsetX = 0;
        s32 offsetY = 0;
        s32 offsetZ = 0;
        float frequence = 8;
        float persistence = 0.4;
    } s_perlinData;


    VkSubBuffer outputDataBuf =
        vkctx.allocate(sizeof(s_outputData), DescriptorType::UNIFORM_BUFFER,
                       MemoryType::CPU_WRITES);
    VkSubBuffer hashBuf =
        vkctx.allocate(hash.size() * sizeof(u32),
                       DescriptorType::STORAGE_BUFFER, MemoryType::CPU_WRITES);

    outputDataBuf.setData(&s_outputData);
    hashBuf.setData(&hash[0]);

    // create the global buffer before profiler start recording,
    // so that the allocation time is not taken into account
    VkSubBuffer dummy = vkctx.allocate(1, DescriptorType::STORAGE_BUFFER,
                                       MemoryType::CPU_READS);

    Profiler profiler;
    profiler.endStartSection("setup");
    // Process tiles
    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);

        tileData._vkData = std::make_unique<ProxyGroundDataPrivate::VkData>(
            ProxyGroundDataPrivate::VkData{
                vkctx.allocate(sizeof(s_perlinData),
                               DescriptorType::UNIFORM_BUFFER,
                               MemoryType::CPU_WRITES),
                vkctx.allocate(bufferSize, DescriptorType::STORAGE_BUFFER,
                               MemoryType::CPU_READS),
                VkWorker()});

        s_perlinData.offsetX = tc._pos.x * 8;
        s_perlinData.offsetY = tc._pos.y * 8;
        tileData._vkData->_heightData.setData(&s_perlinData);

        DescriptorSetVk dset(layout1);
        dset.addDescriptor(0, DescriptorType::UNIFORM_BUFFER, outputDataBuf);
        dset.addDescriptor(1, DescriptorType::UNIFORM_BUFFER,
                           tileData._vkData->_heightData);
        dset.addDescriptor(2, DescriptorType::STORAGE_BUFFER, hashBuf);
        dset.addDescriptor(3, DescriptorType::STORAGE_BUFFER,
                           tileData._vkData->_height);

        VkWorker &worker = tileData._vkData->_worker;
        worker.bindCommand(pipeline1, dset);
        worker.dispatchCommand(bufferWidth / workgroupSize,
                               bufferHeight / workgroupSize, 1);
        worker.endCommandRecording();

        worker.run();
    }
    profiler.endStartSection("waitFences");

    // Collecting images
    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);
        tileData._vkData->_worker.waitForCompletion();
    }
    profiler.endStartSection("collect");

    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);

        float *buffer = new float[bufferCount];
        tileData._vkData->_height.getData(buffer);
        Image img(tileSystem._bufferRes.x, tileSystem._bufferRes.y,
                  ImageType::GREYSCALE);

        for (u32 y = 0; y < img.height(); ++y) {
            for (u32 x = 0; x < img.width(); ++x) {
                u32 pos = (y * img.width() + x);
                img.grey(x, y).setLevelf(buffer[pos]);
            }
        }

        ItemKey key = ItemKeys::inObject(
            tc._pos.x + 0xFFFF * tc._pos.y); // TODO better id ?
                                             // _tileSystem.getId() ?
        imageChan.put(key, img);
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
