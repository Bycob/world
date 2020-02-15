#include "VkwMultilayerGroundTexture.h"

#include <list>

#include <world/core/GridStorage.h>

#include "wrappers/VkwImage.h"
#include "wrappers/VkwTextureGenerator.h"
#include "wrappers/VkwRandomTexture.h"
#include "wrappers/VkwMemoryHelper.h"
#include "wrappers/VkwWorker.h"
#include "wrappers/VkwPerlin.h"

#include "VkwGroundTextureGenerator.h"

namespace world {

class MultilayerElement : public IGridElement {
public:
    // Inputs
    VkwImage _terrain;
    VkwImage _slope;
    std::vector<VkwImage> _textures;

    // Outputs
    std::vector<VkwImage> _distributions;
    VkwImage _finalTexture;

    std::unique_ptr<VkwGraphicsWorker> _worker;

    // Reference to terrain image
    Image *_image;

    MultilayerElement(int terrainSize, int texSize, int layerCount,
                      Image *image)
            : _terrain(terrainSize, terrainSize, VkwImageUsage::TEXTURE,
                       vk::Format::eR32Sfloat),
              _slope(terrainSize, terrainSize, VkwImageUsage::TEXTURE,
                     vk::Format::eR32Sfloat),
              _finalTexture(texSize, texSize, VkwImageUsage::OFFSCREEN_RENDER,
                            vk::Format::eR32G32B32A32Sfloat),
              _image(image) {

        _terrain.setSamplerAddressMode(vk::SamplerAddressMode::eClampToEdge);

        for (int i = 0; i < layerCount; ++i) {
            _distributions.emplace_back(texSize, texSize,
                                        VkwImageUsage::OFFSCREEN_RENDER,
                                        vk::Format::eR32Sfloat);
        }
    }
};

class MultilayerGroundTexturePrivate {
public:
    std::mt19937 _rng;
    std::vector<DistributionParams> _layers;
    VkwGroundTextureGenerator _texGenerator;

    std::list<TileCoordinates> _queue;
    GridStorage<MultilayerElement> _storage;

    u32 _texWidth = 1024;

    VkwSubBuffer _perlinHash;


    MultilayerGroundTexturePrivate()
            : _rng(std::random_device()()),
              _perlinHash(VkwPerlin::createPerlinHash()) {}

    /** Wait for texture generation to be finished and transition to a
     * texture usage. Then assign every textures to the element.
     * @param width The width of the texture to be generated at this lod. */
    void getTileTextures(const TileCoordinates &tc, MultilayerElement &elem,
                         float width);

    VkwPerlinParameters getPerlinParameters(const TileCoordinates &tc,
                                            int layer);

    void process(Terrain &terrain, Image &image, const TileCoordinates &tc);
};

VkwMultilayerGroundTexture::VkwMultilayerGroundTexture()
        : _internal(new MultilayerGroundTexturePrivate()) {}

VkwMultilayerGroundTexture::~VkwMultilayerGroundTexture() { delete _internal; }

void VkwMultilayerGroundTexture::addDefaultLayers() {
    // Rock
    addLayer(DistributionParams{-1, 0, 1, 2, // h
                                -1, 0, 1, 2, // dh
                                0, 1, 0, 1, 0.2},
             "texture-rock.frag");

    // Sand
    addLayer(DistributionParams{-1, 0, 0.4, 0.45, // h
                                -1, 0, 0.4, 0.6,  // dh
                                0, 1, 0, 1, 0.2},
             "texture-sand.frag");

    // Soil
    addLayer(DistributionParams{0.33, 0.4, 0.6, 0.75, // h
                                -1, 0, 0.4, 0.9,      // dh
                                0, 0.85, 0.25, 0.85, 0.2},
             "texture-soil.frag");

    // Grass
    addLayer(DistributionParams{0.33, 0.4, 0.6, 0.7, // h
                                -1, 0, 0.2, 0.6,     // dh
                                0., 1., 0.25, 0.6, 0.2},
             "texture-grass.frag");

    // Snow
    addLayer(DistributionParams{0.65, 0.8, 1, 2, // h
                                -1, 0, 0.5, 0.7, // dh
                                0.0, 1.0, 0, 1., 0.2},
             "texture-snow.frag");
}

void VkwMultilayerGroundTexture::addLayer(
    const DistributionParams &distribution, const std::string &textureShader) {

    _internal->_layers.push_back(distribution);
    _internal->_texGenerator.addLayer(textureShader);
}

void VkwMultilayerGroundTexture::processTerrain(Terrain &terrain) {
    _internal->process(terrain, terrain.getTexture(), {});
    flush();
}

void VkwMultilayerGroundTexture::processTile(ITileContext &context) {
    _internal->process(context.getTile().terrain(), context.getTile().texture(),
                       context.getCoords());
}

void MultilayerGroundTexturePrivate::getTileTextures(const TileCoordinates &tc,
                                                     MultilayerElement &elem,
                                                     float width) {

    if (tc._lod == 0) {
        _texGenerator.setBaseWorldWidth(width);
    }

    for (size_t i = 0; i < _layers.size(); ++i) {
        elem._textures.push_back(_texGenerator.getVkTexture(i, tc._lod));
    }
}

VkwPerlinParameters MultilayerGroundTexturePrivate::getPerlinParameters(
    const TileCoordinates &tc, int layer) {

    VkwPerlinParameters params;
    const int frequency = 8;
    params.octaves = u32(tc._lod + 4);
    params.octaveRef = u32(tc._lod);
    params.offsetX = tc._pos.x * frequency;
    params.offsetY = tc._pos.y * frequency;
    params.offsetZ = layer;
    params.frequency = frequency;

    return params;
}

void MultilayerGroundTexturePrivate::process(Terrain &terrain, Image &image,
                                             const TileCoordinates &tc) {

    const u32 terrainRes = terrain.getResolution();
    const u32 imgWidth = image.width();
    vec3d terrainDims = terrain.getBoundingBox().getDimensions();

    auto &elem =
        _storage.getOrCreate(tc, terrainRes, imgWidth, _layers.size(), &image);
    getTileTextures(tc, elem, terrainDims.x);

    VkwMemoryHelper::terrainToGPU(terrain, elem._terrain);
    // slope is not used yet

    elem._worker = std::make_unique<VkwGraphicsWorker>();

    for (size_t i = 0; i < _layers.size(); ++i) {
        DistributionParams distribParams = _layers[i];
        distribParams.slopeFactor =
            static_cast<float>(terrainDims.z * terrainRes / terrainDims.x);

        VkwDescriptorSetLayout layout({0, 1, 3}, {256});
        layout.addBinding(DescriptorType::IMAGE, 2);

        VkwGraphicsPipeline pipeline(layout);
        pipeline.enableVertexBuffer(false);
        pipeline.setBuiltinShader(VkwShaderType::VERTEX,
                                  "generic-texture.vert");
        pipeline.setBuiltinShader(VkwShaderType::FRAGMENT,
                                  "distribution-height.frag");

        VkwDescriptorSet dset(layout);
        dset.addUniformStruct(0, distribParams);
        dset.addUniformStruct(1, getPerlinParameters(tc, i));

        dset.addDescriptor(2, elem._terrain);
        float imgSizes[] = {static_cast<float>(imgWidth),
                            static_cast<float>(imgWidth)};
        dset.addUniformStruct(3, imgSizes);
        dset.addDescriptor(256, _perlinHash);

        VkwRenderPass renderPass(elem._distributions[i]);
        pipeline.setRenderPass(renderPass);

        elem._worker->beginRenderPass(renderPass);
        elem._worker->bindCommand(pipeline, dset);
        elem._worker->draw(6);
        elem._worker->endRenderPass();

        // TODO transitions to textures
    }

    VkwDescriptorSetLayout layout({0}, {});
    layout.addBinding(DescriptorType::IMAGE, 1, elem._distributions.size());
    layout.addBinding(DescriptorType::IMAGE, 2, elem._textures.size());

    VkwGraphicsPipeline pipeline(layout);
    pipeline.enableVertexBuffer(false);
    pipeline.setBuiltinShader(VkwShaderType::VERTEX, "generic-texture.vert");
    pipeline.setBuiltinShader(VkwShaderType::FRAGMENT,
                              "multilayer-texture.frag");

    VkwDescriptorSet dset(layout);

    struct {
        float offsetX;
        float offsetY;
        float sizeX;
        float sizeY;
        uint32_t texCount;
    } metadata;
    metadata.sizeY = metadata.sizeX = static_cast<float>(imgWidth) / _texWidth;
    float empty;
    metadata.offsetX = std::modf(tc._pos.x * metadata.sizeX, &empty);
    metadata.offsetY = std::modf(tc._pos.y * metadata.sizeY, &empty);
    metadata.texCount = elem._textures.size();
    dset.addUniformStruct(0, metadata);
    dset.addTextureArray(1, elem._distributions);
    dset.addTextureArray(2, elem._textures);

    VkwRenderPass renderPass(elem._finalTexture);
    pipeline.setRenderPass(renderPass);

    // Merge all layers into one
    elem._worker->beginRenderPass(renderPass);
    elem._worker->bindCommand(pipeline, dset);
    elem._worker->draw(6);
    elem._worker->endRenderPass();
    elem._worker->endCommandRecording();

    elem._worker->run();
    _queue.push_back(tc);
}

void VkwMultilayerGroundTexture::flush() {
    for (const TileCoordinates &tc : _internal->_queue) {
        auto &elem = _internal->_storage.get(tc);
        elem._worker->waitForCompletion();
        VkwMemoryHelper::GPUToImage(elem._finalTexture, *elem._image, 4);

        // Dealocate resources
        elem._worker = nullptr;
        elem._image = nullptr;
    }

    _internal->_queue.clear();
}

GridStorageBase *VkwMultilayerGroundTexture::getStorage() {
    return &_internal->_storage;
}

} // namespace world
