#include "VkwGroundTextureGenerator.h"

#include <memory>
#include <vector>
#include <vkworld/wrappers/VkwMemoryHelper.h>

#include "wrappers/VkwImage.h"
#include "wrappers/VkwWorker.h"
#include "wrappers/VkwRandomTexture.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITextureProvider, VkwGroundTextureGenerator,
                           "VkwGroundTextureGenerator")

struct LodTextures {
    float _width = 0;
    std::unique_ptr<VkwGraphicsWorker> _texWorker;
    std::vector<VkwImage> _layerTextures;
    std::vector<Image> _layerImages;
};

class VkwGroundTextureGeneratorPrivate {
public:
    u32 _texWidth = 1024;
    std::vector<std::string> _layers;
    VkwRandomTexture _randTex;

    /// Generated textures
    std::map<int, LodTextures> _lodTextures;


    void launchTextureGeneration(LodTextures &t);
};

VkwGroundTextureGenerator::VkwGroundTextureGenerator()
        : _internal(new VkwGroundTextureGeneratorPrivate()) {}

VkwGroundTextureGenerator::~VkwGroundTextureGenerator() { delete _internal; }


void VkwGroundTextureGenerator::addLayer(const std::string &textureShader) {
    _internal->_layers.push_back(textureShader);
}

size_t VkwGroundTextureGenerator::getLayerCount() {
    return _internal->_layers.size();
}

VkwImage &VkwGroundTextureGenerator::getVkTexture(int layer, int lod) {
    // FIXME cpu was set to false, but we actually need (...)
    // to set it to true to take advantage of the cache
    auto &lodTex = getOrCreate(lod, true);
    return lodTex._layerTextures.at(layer);
}

Image &VkwGroundTextureGenerator::getTexture(int layer, int lod) {
    auto &lodTex = getOrCreate(lod, true);
    return lodTex._layerImages.at(layer);
}

void VkwGroundTextureGenerator::write(WorldFile &wf) const {
    wf.addUint("texWidth", _internal->_texWidth);
    wf.addArray("layers");

    for (auto &layer : _internal->_layers) {
        WorldFile layerWf;
        layerWf.addString("shader", layer);
        wf.addToArray("layers", std::move(layerWf));
    }
}

void VkwGroundTextureGenerator::read(const WorldFile &wf) {
    wf.readUintOpt("texWidth", _internal->_texWidth);

    for (auto it = wf.readArray("layers"); !it.end(); ++it) {
        _internal->_layers.push_back(it->readString("shader"));
    }
}

LodTextures &VkwGroundTextureGenerator::getOrCreate(int lod, bool cpu) {
    auto it = _internal->_lodTextures.insert({lod, LodTextures{}});
    LodTextures &t = it.first->second;

    if (it.second) {
        t._width = getWidth(lod);
        _internal->launchTextureGeneration(t);
        t._texWorker->waitForCompletion();
    }

    // TODO transitions to textures ?

    if (cpu && t._layerTextures.size() != t._layerImages.size()) {
        t._layerImages.clear();

        for (VkwImage &img : t._layerTextures) {
            t._layerImages.push_back(VkwMemoryHelper::GPUToImage(img));
            _cache.saveImage(getImageId(t._layerImages.size() - 1, lod),
                             t._layerImages.back());
        }
    }
    return t;
}

void VkwGroundTextureGeneratorPrivate::launchTextureGeneration(LodTextures &t) {
    t._texWorker = std::make_unique<VkwGraphicsWorker>();

    for (const auto &layer : _layers) {
        VkwImage image(_texWidth, _texWidth, VkwImageUsage::OFFSCREEN_RENDER);
        t._layerTextures.push_back(image);

        // Generate image
        VkwDescriptorSetLayout layout({0}, {});
        layout.addBinding(DescriptorType::IMAGE, 1);

        VkwGraphicsPipeline pipeline(layout);
        pipeline.enableVertexBuffer(false);
        pipeline.setBuiltinShader(VkwShaderType::VERTEX,
                                  "generic-texture.vert");
        pipeline.setBuiltinShader(VkwShaderType::FRAGMENT, layer);

        VkwDescriptorSet dset(layout);

        struct {
            float offsetX = 0;
            float offsetY = 0;
            float width;
            float height;
        } sizes;
        sizes.width = t._width;
        sizes.height = t._width;

        dset.addUniformStruct(0, sizes);
        dset.addDescriptor(1, _randTex.get());

        VkwRenderPass renderPass(image);
        pipeline.setRenderPass(renderPass);

        t._texWorker->beginRenderPass(renderPass);
        t._texWorker->bindCommand(pipeline, dset);
        t._texWorker->draw(6);
        t._texWorker->endRenderPass();
    }

    t._texWorker->endCommandRecording();
    t._texWorker->run();
}

float VkwGroundTextureGenerator::getWidth(int lod) {
    return _basePixelSize * _internal->_texWidth * powi(2.0f, -lod);
}
} // namespace world
