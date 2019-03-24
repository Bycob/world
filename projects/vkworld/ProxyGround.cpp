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

    DescriptorSetLayoutVk layoutPerlin;
    layoutPerlin.addBinding(DescriptorType::UNIFORM_BUFFER, 0);
    layoutPerlin.addBinding(DescriptorType::UNIFORM_BUFFER, 1);
    layoutPerlin.addBinding(DescriptorType::STORAGE_BUFFER, 2);
    layoutPerlin.addBinding(DescriptorType::STORAGE_BUFFER, 3);

    ComputePipeline pipelinePerlin(layoutPerlin, "noise-perlin");

    DescriptorSetLayoutVk layoutUpscale;
    layoutUpscale.addBinding(DescriptorType::UNIFORM_BUFFER, 0);
    layoutUpscale.addBinding(DescriptorType::UNIFORM_BUFFER, 1);
    layoutUpscale.addBinding(DescriptorType::STORAGE_BUFFER, 2);
    layoutUpscale.addBinding(DescriptorType::STORAGE_BUFFER, 3);

    ComputePipeline pipelineUpscale(layoutUpscale, "upscale");

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

    // Perlin generation (TEMP)
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
        u32 octaves = 9;
        u32 octaveRef = 0;
        s32 offsetX = 0;
        s32 offsetY = 0;
        s32 offsetZ = 0;
        float frequence = 8;
        float persistence = 0.4;
    } s_perlinData;


    VkSubBuffer outputDataBuf =
        vkctx.allocate(sizeof(s_outputData), DescriptorType::UNIFORM_BUFFER,
                       MemoryType::CPU_WRITES);

    VkSubBuffer perlinDataBuf =
        vkctx.allocate(sizeof(s_perlinData), DescriptorType::UNIFORM_BUFFER,
                       MemoryType::CPU_WRITES);

    VkSubBuffer hashBuf =
        vkctx.allocate(hash.size() * sizeof(u32),
                       DescriptorType::STORAGE_BUFFER, MemoryType::CPU_WRITES);

    VkSubBuffer perlinBuf = vkctx.allocate(
        bufferSize, DescriptorType::STORAGE_BUFFER, MemoryType::GPU_ONLY);

    outputDataBuf.setData(&s_outputData);
    perlinDataBuf.setData(&s_perlinData);
    hashBuf.setData(&hash[0]);

    DescriptorSetVk dsetPerlin(layoutPerlin);
    dsetPerlin.addDescriptor(0, DescriptorType::UNIFORM_BUFFER, outputDataBuf);
    dsetPerlin.addDescriptor(1, DescriptorType::UNIFORM_BUFFER, perlinDataBuf);
    dsetPerlin.addDescriptor(2, DescriptorType::STORAGE_BUFFER, hashBuf);
    dsetPerlin.addDescriptor(3, DescriptorType::STORAGE_BUFFER, perlinBuf);

    VkWorker worker;
    worker.bindCommand(pipelinePerlin, dsetPerlin);
    worker.dispatchCommand(bufferWidth / workgroupSize,
                           bufferHeight / workgroupSize, 1);
    worker.endCommandRecording();

    worker.run();
    worker.waitForCompletion();

    Profiler profiler;
    profiler.endStartSection("setup");
    // Process tiles
    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);

        struct {
            u32 srcWidth;
            u32 srcHeight;
            u32 srcDepth = 1;
            u32 padding0;

            u32 offsetX;
            u32 offsetY;
            u32 offsetZ = 0;
            u32 padding1;

            u32 sizeX;
            u32 sizeY;
            u32 sizeZ = 0;
        } s_upscaleData;

        // vec3d bboxDims = bbox.getDimensions();
        // TODO find this variables with a good method
        vec2i tileCount(13, 13);
        vec2i tileOrigin(-6, -6);
        vec2i tileSize(bufferWidth / tileCount.x, bufferHeight / tileCount.y);

        s_upscaleData.srcWidth = bufferWidth;
        s_upscaleData.srcHeight = bufferHeight;
        s_upscaleData.offsetX = (tc._pos.x - tileOrigin.x) * tileSize.x;
        s_upscaleData.offsetY = (tc._pos.y - tileOrigin.y) * tileSize.y;
        s_upscaleData.sizeX = tileSize.x;
        s_upscaleData.sizeY = tileSize.y;

        tileData._vkData = std::make_unique<ProxyGroundDataPrivate::VkData>(
            ProxyGroundDataPrivate::VkData{
                vkctx.allocate(sizeof(s_upscaleData),
                               DescriptorType::UNIFORM_BUFFER,
                               MemoryType::CPU_WRITES),
                vkctx.allocate(bufferSize, DescriptorType::STORAGE_BUFFER,
                               MemoryType::CPU_READS),
                VkWorker()});

        tileData._vkData->_upscaleData.setData(&s_upscaleData);

        DescriptorSetVk dset(layoutUpscale);
        dset.addDescriptor(0, DescriptorType::UNIFORM_BUFFER, outputDataBuf);
        dset.addDescriptor(1, DescriptorType::UNIFORM_BUFFER,
                           tileData._vkData->_upscaleData);
        dset.addDescriptor(2, DescriptorType::STORAGE_BUFFER, perlinBuf);
        dset.addDescriptor(3, DescriptorType::STORAGE_BUFFER,
                           tileData._vkData->_height);

        VkWorker &worker = tileData._vkData->_worker;
        worker.bindCommand(pipelineUpscale, dset);
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
