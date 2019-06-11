#include "MultilayerGroundTexture.h"

#include <list>

#include "Vulkan.h"
#include "VkwMemoryHelper.h"
#include "VkSubBuffer.h"
#include "VkWorker.h"
#include "ComputePipeline.h"

namespace world {

struct ProcessUnit {
    Image &_textureImg;

    VkwSubBuffer _terrainHeight;
    VkwSubBuffer _terrainSlope;
    // Same buffers after upscaling
    VkwSubBuffer _terrainHeightUp;
    VkwSubBuffer _terrainSlopeUp;

    VkwSubBuffer _derivParams;
    VkwSubBuffer _upscaleHeightParams;
    VkwSubBuffer _upscaleSlopeParams;
    /// Buffer used only by the layers. It's the same for every layers.
    VkwSubBuffer _sizeParams;

    VkwSubBuffer _random;

    struct Layer {
        VkwSubBuffer _distributionParams;
        VkwSubBuffer _distributionPerlinParams;
        VkwSubBuffer _distribution;
        VkwSubBuffer _textureParams;
    };
    std::vector<Layer> _layers;

    VkwSubBuffer _texture;

    std::unique_ptr<VkwWorker> _worker;


    ProcessUnit(Image &img) : _textureImg(img) {}
};

class MultilayerGroundTexturePrivate {
public:
    struct LayerInfo {
        DistributionParams _distributionParams;
        std::string _textureShader;
    };
    std::vector<LayerInfo> _layers;

    std::list<ProcessUnit> _units;
};

MultilayerGroundTexture::MultilayerGroundTexture()
        : _internal(new MultilayerGroundTexturePrivate()), _rng(time(NULL)) {}

MultilayerGroundTexture::~MultilayerGroundTexture() { delete _internal; }

void MultilayerGroundTexture::addDefaultLayers() {
    // Rock
    addLayer(DistributionParams{-1, 0, 1, 2, // h
                                -1, 0, 1, 2, // dh
                                0, 1, 0, 1, 0.2},
             "texture-rock");

    // Sand
    addLayer(DistributionParams{-1, 0, 0.4, 0.45, // h
                                -1, 0, 0.4, 0.6,  // dh
                                0, 1, 0, 1, 0.2},
             "texture-sand");

    // Soil
    addLayer(DistributionParams{0.33, 0.4, 0.6, 0.75, // h
                                -1, 0, 0.4, 0.9,      // dh
                                0, 0.85, 0.25, 0.85, 0.2},
             "texture-soil");

    // Grass
    addLayer(DistributionParams{0.33, 0.4, 0.6, 0.7, // h
                                -1, 0, 0.2, 0.6,     // dh
                                0., 1., 0.25, 0.6, 0.2},
             "texture-grass");

    // Snow
    addLayer(DistributionParams{0.65, 0.8, 1, 2, // h
                                -1, 0, 0.5, 0.7, // dh
                                0.0, 1.0, 0, 1., 0.2},
             "texture-snow");
}

void MultilayerGroundTexture::addLayer(const DistributionParams &distribution,
                                       const std::string &textureShader) {
    _internal->_layers.push_back({distribution, textureShader});
}

void MultilayerGroundTexture::processTerrain(Terrain &terrain) {
    process(terrain, terrain.getTexture(), {0, 0}, 0);
    flush();
}

void MultilayerGroundTexture::processTile(ITileContext &context) {
    Terrain &terrain = context.getTerrain();
    Image &img = context.getTexture();
    process(terrain, img, context.getTileCoords(), context.getParentCount());
}

void MultilayerGroundTexture::process(Terrain &terrain, Image &img,
                                      vec2i tileCoords, int parentGap) {
    // Assume that terrains and images are squared
    const u32 groupSize = 32;

    float sizeFactor = float(terrain.getResolution()) / float(img.width());
    const u32 terrainRes = terrain.getResolution();
    const u32 terrainCount = terrainRes * terrainRes;
    const u32 terrainSize = terrainCount * sizeof(float);
    vec3d terrainDims = terrain.getBoundingBox().getDimensions();

    const u32 derivRes = terrainRes - 1;
    const u32 derivCount = derivRes * derivRes;
    const u32 derivSize = derivCount * sizeof(float);
    const u32 derivGroupCount = (derivRes - 1) / groupSize + 1;

    const u32 imgSideCount = img.width();
    const u32 imgPixCount = imgSideCount * imgSideCount;
    const u32 imgPixSize = imgPixCount * sizeof(float);
    const u32 imgCount = imgPixCount * 4;
    const u32 imgSize = imgCount * sizeof(float);
    const u32 imgGroupCount = (imgSideCount - 1) / groupSize + 1;

    _internal->_units.emplace_back(img);
    ProcessUnit &unit = _internal->_units.back();

    // Random
    std::vector<u32> random(256);
    std::iota(random.begin(), random.end(), 0);
    std::shuffle(random.begin(), random.end(), _rng);
    random.insert(random.end(), random.begin(), random.end());

    // Vulkan setup
    auto &vkctx = Vulkan::context();

    // Needed shaders:
    // - derivation * 1
    // - upscale * 2
    // + distribution * n_layers
    // + texture * n_layers

    // clang-format off
    VkwDescriptorSetLayout layoutDeriv({0}, {1, 2});
    VkwDescriptorSetLayout &layoutUpscale = layoutDeriv;

    // 1 = distribution params; 2 = perlin; 3 = height buffer; 4 = dheight buffer; 256 = random;
    VkwDescriptorSetLayout layoutDistrib({0, 1, 2}, {3, 4, 5, 256});

    // 1 = custom texture parameters
    VkwDescriptorSetLayout layoutTexture({0, 1}, {2, 3, 256});

    // Pipelines
    VkwComputePipeline derivPipeline(layoutDeriv, "derivation");
    VkwComputePipeline upscalePipeline(layoutUpscale, "upscale");

    std::vector<VkwComputePipeline> distributionPipelines;
    std::vector<VkwComputePipeline> texturePipelines;

    for (auto &layerInfo : _internal->_layers) {
        distributionPipelines.emplace_back(layoutDistrib,
                                           "distribution-height");
        texturePipelines.emplace_back(layoutTexture, layerInfo._textureShader);

        unit._layers.emplace_back();
    }

    // Buffers
    unit._terrainHeight = vkctx.allocate(terrainSize, DescriptorType::STORAGE_BUFFER, MemoryType::CPU_WRITES);
    unit._terrainSlope = vkctx.allocate(derivSize, DescriptorType::STORAGE_BUFFER, MemoryType::GPU_ONLY);
    unit._terrainHeightUp = vkctx.allocate(imgPixSize, DescriptorType::STORAGE_BUFFER, MemoryType::GPU_ONLY);
    unit._terrainSlopeUp = vkctx.allocate(imgPixSize, DescriptorType::STORAGE_BUFFER, MemoryType::GPU_ONLY);
    unit._texture = vkctx.allocate(imgSize, DescriptorType::STORAGE_BUFFER, MemoryType::CPU_READS);
    unit._random = vkctx.allocate(static_cast<u32>(random.size()) * sizeof(u32), DescriptorType::STORAGE_BUFFER, MemoryType::CPU_WRITES);

    VkwMemoryHelper::terrainToGPU(terrain, unit._terrainHeight);
    unit._random.setData(random.data());

    struct {
        u32 width;
        u32 height;
        u32 depth = 1;
    } derivParamsStruct;
    derivParamsStruct.width = terrainRes;
    derivParamsStruct.height = terrainRes;

    unit._derivParams = vkctx.allocate(sizeof(derivParamsStruct), DescriptorType::UNIFORM_BUFFER, MemoryType::CPU_WRITES);
    unit._derivParams.setData(&derivParamsStruct);

    struct {
        u32 dstWidth;
        u32 dstHeight;
        u32 dstDepth = 1;
        u32 fill0;

        u32 srcWidth;
        u32 srcHeight;
        u32 srcDepth = 1;
        u32 fill1;

        u32 srcOffsetX = 0;
        u32 srcOffsetY = 0;
        u32 srcOffsetZ = 0;
        u32 fill2;

        u32 srcSizeX;
        u32 srcSizeY;
        u32 srcSizeZ = 1;
    } upscaleStruct;
    upscaleStruct.dstWidth = static_cast<u32>(img.width());
    upscaleStruct.dstHeight = static_cast<u32>(img.height());
    upscaleStruct.srcWidth = upscaleStruct.srcHeight = upscaleStruct.srcSizeX = upscaleStruct.srcSizeY = terrainRes;

    unit._upscaleHeightParams = vkctx.allocate(sizeof(upscaleStruct), DescriptorType::UNIFORM_BUFFER, MemoryType::CPU_WRITES);
    unit._upscaleHeightParams.setData(&upscaleStruct);

    upscaleStruct.srcWidth = upscaleStruct.srcHeight = upscaleStruct.srcSizeX = upscaleStruct.srcSizeY = derivRes;

    unit._upscaleSlopeParams = vkctx.allocate(sizeof(upscaleStruct), DescriptorType::UNIFORM_BUFFER, MemoryType::CPU_WRITES);
    unit._upscaleSlopeParams.setData(&upscaleStruct);

    // Here is a little hack: we have the size of the image already embedded at the beggining of our upscale struct.
    unit._sizeParams = unit._upscaleHeightParams;

    // DescriptorSet
    VkwDescriptorSet derivDset(layoutDeriv);
    derivDset.addDescriptor(0, unit._derivParams);
    derivDset.addDescriptor(1, unit._terrainHeight);
    derivDset.addDescriptor(2, unit._terrainSlope);

    VkwDescriptorSet upscaleHeightDset(layoutUpscale);
    upscaleHeightDset.addDescriptor(0, unit._upscaleHeightParams);
    upscaleHeightDset.addDescriptor(1, unit._terrainHeight);
    upscaleHeightDset.addDescriptor(2, unit._terrainHeightUp);

    VkwDescriptorSet upscaleSlopeDset(layoutUpscale);
    upscaleSlopeDset.addDescriptor(0, unit._upscaleSlopeParams);
    upscaleSlopeDset.addDescriptor(1, unit._terrainSlope);
    upscaleSlopeDset.addDescriptor(2, unit._terrainSlopeUp);

    // Worker
    unit._worker = std::make_unique<VkwWorker>();

    unit._worker->bindCommand(derivPipeline, derivDset);
    unit._worker->dispatchCommand(derivGroupCount, derivGroupCount, 1);
    unit._worker->bindCommand(upscalePipeline, upscaleHeightDset);
    unit._worker->dispatchCommand(imgGroupCount, imgGroupCount, 1);
    unit._worker->bindCommand(upscalePipeline, upscaleSlopeDset);
    unit._worker->dispatchCommand(imgGroupCount, imgGroupCount, 1);

    int i = 0;

    for (auto &layerInfo : _internal->_layers) {
        auto &layer = unit._layers[i];

        // Buffers
        layer._distribution = vkctx.allocate(imgPixSize, DescriptorType::STORAGE_BUFFER, MemoryType::GPU_ONLY);

        layer._distributionParams = vkctx.allocate(sizeof(DistributionParams), DescriptorType::UNIFORM_BUFFER, MemoryType::CPU_WRITES);
        layerInfo._distributionParams.slopeFactor = terrainDims.z * terrainRes / terrainDims.x;
        layer._distributionParams.setData(&layerInfo._distributionParams);

        struct {
            u32 octaves;
            u32 octaveRef;
            s32 offsetX;
            s32 offsetY;
            s32 offsetZ = 0;
            float frequency;
            float persistence = 1.2f;
        } distributionPerlinStruct;
        const int frequency = 8;
        distributionPerlinStruct.octaves = u32(parentGap + 4);
        distributionPerlinStruct.octaveRef = u32(parentGap);
        distributionPerlinStruct.offsetX = tileCoords.x * frequency;
        distributionPerlinStruct.offsetY = tileCoords.y * frequency;
        distributionPerlinStruct.frequency = frequency;
        layer._distributionPerlinParams = vkctx.allocate(sizeof(distributionPerlinStruct), DescriptorType::UNIFORM_BUFFER, MemoryType::CPU_WRITES);
        layer._distributionPerlinParams.setData(&distributionPerlinStruct);

        VkwDescriptorSet distributionDset(layoutDistrib);
        distributionDset.addDescriptor(0, unit._sizeParams);
        distributionDset.addDescriptor(1, layer._distributionParams);
        distributionDset.addDescriptor(2, layer._distributionPerlinParams);
        distributionDset.addDescriptor(3, unit._terrainHeightUp);
        distributionDset.addDescriptor(4, unit._terrainSlopeUp);
        distributionDset.addDescriptor(5, layer._distribution);
        distributionDset.addDescriptor(256, unit._random);

        unit._worker->bindCommand(distributionPipelines[i], distributionDset);
        unit._worker->dispatchCommand(imgGroupCount, imgGroupCount, 1);

        struct {
            float offsetX;
            float offsetY;
            float sizeX;
            float sizeY;
        } textureStruct;

        const float tileSize = terrainDims.x;
        textureStruct.sizeX = tileSize;
        textureStruct.sizeY = tileSize;
        textureStruct.offsetX = tileCoords.x * textureStruct.sizeX;
        textureStruct.offsetY = tileCoords.y * textureStruct.sizeY;
        layer._textureParams = vkctx.allocate(sizeof(textureStruct), DescriptorType::UNIFORM_BUFFER, MemoryType::CPU_WRITES);
        layer._textureParams.setData(&textureStruct);

        VkwDescriptorSet textureDset(layoutTexture);
        textureDset.addDescriptor(0, unit._sizeParams);
        textureDset.addDescriptor(1, layer._textureParams);
        textureDset.addDescriptor(2, layer._distribution);
        textureDset.addDescriptor(3, unit._texture);
        textureDset.addDescriptor(256, unit._random);

        unit._worker->bindCommand(texturePipelines[i], textureDset);
        unit._worker->dispatchCommand(imgGroupCount, imgGroupCount, 1);

        i++;
    }

    unit._worker->endCommandRecording();
    unit._worker->run();

    // clang-format on
}

void MultilayerGroundTexture::flush() {
    for (auto &unit : _internal->_units) {
        unit._worker->waitForCompletion();
        VkwMemoryHelper::GPUToImage(unit._texture, unit._textureImg, 4);
        // VkwMemoryHelper::GPUToImage(unit._terrainHeightUp, unit._textureImg);
    }

    _internal->_units.clear();
}

} // namespace world