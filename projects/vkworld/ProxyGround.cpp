#include "ProxyGround.h"
#include "ProxyGround_p.h"

#include <set>

#include <world/assets/Image.h>
#include <world/math/Perlin.h>
#include <world/core/Profiler.h>

#include "wrappers/Vulkan.h"
#include "wrappers/VkwDescriptorSet.h"
#include "wrappers/VkwComputePipeline.h"

namespace world {

ProxyGround::ProxyGround(f64 width, u32 resolution) {
    const u32 tileSize = 128;
    const u32 tileCount = resolution / tileSize;
    _internal = new ProxyGroundPrivate(width / tileCount, tileSize, tileCount);

    _internal->_layers.emplace_back(
        ProxyGroundPrivate::LayerInfo{"distribution-default", "texture-soil"});
    _internal->_layers.emplace_back(
        ProxyGroundPrivate::LayerInfo{"distribution-default", "texture-grass"});
}

ProxyGround::~ProxyGround() { delete _internal; }

VkwSubBuffer createTestPerlinBuffer() {
    auto &vkctx = Vulkan::context();

    VkwDescriptorSetLayout layout22; // 2 uniforms 2 storage
    layout22.addBinding(DescriptorType::UNIFORM_BUFFER, 0);
    layout22.addBinding(DescriptorType::UNIFORM_BUFFER, 1);
    layout22.addBinding(DescriptorType::STORAGE_BUFFER, 256);
    layout22.addBinding(DescriptorType::STORAGE_BUFFER, 3);

    VkwComputePipeline perlinPipeline(layout22, "noise-perlin");

    Perlin perlin;
    std::vector<u32> hash;

    for (auto i : perlin.getHash()) {
        hash.push_back(static_cast<u32>(i));
    }

    const u32 bufferWidth = 128;
    const u32 bufferHeight = 128;
    const u32 bufferCount = bufferWidth * bufferHeight;
    const u32 bufferSize = bufferCount * sizeof(float);

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


    VkwSubBuffer outputDataBuf =
        vkctx.allocate(sizeof(s_outputData), DescriptorType::UNIFORM_BUFFER,
                       MemoryUsage::CPU_WRITES);

    VkwSubBuffer perlinDataBuf =
        vkctx.allocate(sizeof(s_perlinData), DescriptorType::UNIFORM_BUFFER,
                       MemoryUsage::CPU_WRITES);

    VkwSubBuffer hashBuf =
        vkctx.allocate(hash.size() * sizeof(u32),
                       DescriptorType::STORAGE_BUFFER, MemoryUsage::CPU_WRITES);

    VkwSubBuffer perlinBuf = vkctx.allocate(
        bufferSize, DescriptorType::STORAGE_BUFFER, MemoryUsage::GPU_ONLY);

    outputDataBuf.setData(&s_outputData);
    perlinDataBuf.setData(&s_perlinData);
    hashBuf.setData(&hash[0]);

    VkwDescriptorSet perlinDset(layout22);
    perlinDset.addDescriptor(0, outputDataBuf);
    perlinDset.addDescriptor(1, perlinDataBuf);
    perlinDset.addDescriptor(256, hashBuf);
    perlinDset.addDescriptor(3, perlinBuf);

    VkwComputeWorker perlinWorker;
    perlinWorker.bindCommand(perlinPipeline, perlinDset);
    perlinWorker.dispatchCommand(bufferWidth / 32, bufferHeight / 32, 1);
    perlinWorker.endCommandRecording();

    perlinWorker.run();
    perlinWorker.waitForCompletion();

    return perlinBuf;
}

void ProxyGround::collect(ICollector &collector,
                          const IResolutionModel &resolutionModel,
                          const ExplorationContext &ctx) {
    // Init collector
    if (!collector.hasChannel<Image>())
        return;

    auto &imageChan = collector.getChannel<Image>();

    // Get parent (for testing purpose it's only a buffer filled with perlin
    // noise)
    Profiler profiler;
    profiler.endStartSection("perlin generation");

    VkwSubBuffer perlinBuf = createTestPerlinBuffer();
    const u32 parentWidth = 128;
    const u32 parentHeight = 128;

    // Create pipelines for vulkan processing
    auto &vkctx = Vulkan::context();

    profiler.endStartSection("setup");

    VkwDescriptorSetLayout layout22; // 2 uniforms 2 storage
    layout22.addBinding(DescriptorType::UNIFORM_BUFFER, 0);
    layout22.addBinding(DescriptorType::UNIFORM_BUFFER, 1);
    layout22.addBinding(DescriptorType::STORAGE_BUFFER, 2);
    layout22.addBinding(DescriptorType::STORAGE_BUFFER, 3);

    VkwDescriptorSetLayout layoutUpscale({0}, {1, 2});

    VkwComputePipeline upscalePipeline(layoutUpscale, "upscale");

    std::vector<VkwComputePipeline> repartitionPipelines;
    std::vector<VkwComputePipeline> texturePipelines;

    for (auto &layerInfo : _internal->_layers) {
        repartitionPipelines.emplace_back(layout22,
                                          layerInfo._repartitionShader);
        texturePipelines.emplace_back(layout22, layerInfo._textureShader);
    }

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
        auto tileCoords = *it;
        coords.insert(tileCoords);
    }

    std::set<TileCoordinates> toAdd;
    // ...

    // Setup vulkan buffers and variables
    const u32 workgroupSize = 32;
    const u32 dispatchX = bufferWidth / workgroupSize;
    const u32 dispatchY = bufferHeight / workgroupSize;
    const u32 dispatchZ = 1;

    // Output parameters struct
    struct {
        u32 width;
        u32 height;
        u32 depth;
    } s_outputData;

    s_outputData.width = bufferWidth;
    s_outputData.height = bufferHeight;
    s_outputData.depth = 1;

    VkwSubBuffer outputDataBuf =
        vkctx.allocate(sizeof(s_outputData), DescriptorType::UNIFORM_BUFFER,
                       MemoryUsage::CPU_WRITES);

    outputDataBuf.setData(&s_outputData);

    // Process tiles
    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);

        VkwComputeWorker &worker =
            *(tileData._worker = std::make_unique<VkwComputeWorker>());

        // --- UPSCALE
        // vec3d bboxDims = bbox.getDimensions();
        // TODO find this variables with a good method
        vec2i tileCount(13, 13);
        vec2i tileOrigin(-6, -6);
        vec2i tileSize(parentWidth / tileCount.x, parentHeight / tileCount.y);

        struct {
            u32 width;
            u32 height;
            u32 depth = 1;

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

        s_upscaleData.width = bufferWidth;
        s_upscaleData.height = bufferHeight;
        s_upscaleData.srcWidth = parentWidth;
        s_upscaleData.srcHeight = parentHeight;
        s_upscaleData.offsetX = (tc._pos.x - tileOrigin.x) * tileSize.x;
        s_upscaleData.offsetY = (tc._pos.y - tileOrigin.y) * tileSize.y;
        s_upscaleData.sizeX = tileSize.x;
        s_upscaleData.sizeY = tileSize.y;

        tileData._upscaleData = std::make_unique<VkwSubBuffer>(vkctx.allocate(
            sizeof(s_upscaleData), DescriptorType::UNIFORM_BUFFER,
            MemoryUsage::CPU_WRITES)),
        tileData._height = std::make_unique<VkwSubBuffer>(
            vkctx.allocate(bufferSize, DescriptorType::STORAGE_BUFFER,
                           MemoryUsage::CPU_READS));

        tileData._upscaleData->setData(&s_upscaleData);

        VkwDescriptorSet upscaleDset(layoutUpscale);
        upscaleDset.addDescriptor(0, *tileData._upscaleData);
        upscaleDset.addDescriptor(1, perlinBuf);
        upscaleDset.addDescriptor(2, *tileData._height);

        worker.bindCommand(upscalePipeline, upscaleDset);
        worker.dispatchCommand(dispatchX, dispatchY, dispatchZ);

        // Prealloc final texture buffer
        tileData._texture = std::make_unique<VkwSubBuffer>(
            vkctx.allocate(bufferSize * 4, DescriptorType::STORAGE_BUFFER,
                           MemoryUsage::CPU_READS));

        int i = 0;

        for (auto &layerInfo : _internal->_layers) {
            tileData._layers.emplace_back();
            auto &layerData = tileData._layers.back();

            // --- REPARTITION
            struct {

            } s_repartitionData;

            layerData._repartitionData =
                std::make_unique<VkwSubBuffer>(vkctx.allocate(
                    sizeof(s_repartitionData), DescriptorType::UNIFORM_BUFFER,
                    MemoryUsage::CPU_WRITES)),
            layerData._repartition = std::make_unique<VkwSubBuffer>(
                vkctx.allocate(bufferSize, DescriptorType::STORAGE_BUFFER,
                               MemoryUsage::CPU_READS));

            layerData._repartitionData->setData(&s_repartitionData);

            VkwDescriptorSet repartitionDset(layout22);
            repartitionDset.addDescriptor(0, outputDataBuf);
            repartitionDset.addDescriptor(1, *layerData._repartitionData);
            repartitionDset.addDescriptor(2, *tileData._height);
            repartitionDset.addDescriptor(3, *layerData._repartition);

            worker.bindCommand(repartitionPipelines[i], repartitionDset);
            worker.dispatchCommand(dispatchX, dispatchY, dispatchZ);

            // --- TEXTURE
            struct {
                float offsetX;
                float offsetY;
                float sizeX;
                float sizeY;
            } s_textureData;

            s_textureData.sizeX = 1.f / tileCount.x;
            s_textureData.sizeY = 1.f / tileCount.y;
            s_textureData.offsetX = s_textureData.sizeX * tc._pos.x;
            s_textureData.offsetY = s_textureData.sizeY * tc._pos.y;

            layerData._textureData =
                std::make_unique<VkwSubBuffer>(vkctx.allocate(
                    sizeof(s_textureData), DescriptorType::UNIFORM_BUFFER,
                    MemoryUsage::CPU_WRITES));

            layerData._textureData->setData(&s_textureData);


            VkwDescriptorSet textureDset(layout22);
            textureDset.addDescriptor(0, outputDataBuf);
            textureDset.addDescriptor(1, *layerData._textureData);
            textureDset.addDescriptor(2, *layerData._repartition);
            textureDset.addDescriptor(3, *tileData._texture);

            worker.bindCommand(texturePipelines[i], textureDset);
            worker.dispatchCommand(dispatchX, dispatchY, dispatchZ);

            ++i;
        }

        worker.endCommandRecording();
        worker.run();
    }
    profiler.endStartSection("waitFences");

    // Collecting images
    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);
        tileData._worker->waitForCompletion();
    }
    profiler.endStartSection("collect");

    for (const TileCoordinates &tc : coords) {
        auto &tileData = getData(tc);

        float *buffer = new float[bufferCount * 4];
        tileData._texture->getData(buffer);
        Image img(bufferWidth, bufferHeight, ImageType::RGBA);

        for (u32 y = 0; y < img.height(); ++y) {
            for (u32 x = 0; x < img.width(); ++x) {
                u32 pos = (y * img.width() + x) * 4;
                img.rgba(x, y).setf(buffer[pos], buffer[pos + 1],
                                    buffer[pos + 2], buffer[pos + 3]);
            }
        }

        ItemKey key{NodeKeys::fromInt(tc._pos.x + 0xFFFF * tc._pos.y)};
        imageChan.put(key, img);
    }

    profiler.endSection();
    profiler.dump();
}

ProxyGroundDataPrivate &ProxyGround::getData(
    const TileCoordinates &tileCoords) {
    auto found = _internal->_data.find(tileCoords);
    if (found == _internal->_data.end()) {
        return *(_internal->_data[tileCoords] =
                     std::make_unique<ProxyGroundDataPrivate>());
    } else {
        return *found->second;
    }
}

} // namespace world
