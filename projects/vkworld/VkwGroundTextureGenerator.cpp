#include "VkwGroundTextureGenerator.h"

#include <memory>
#include <vector>
#include <vkworld/wrappers/VkwMemoryHelper.h>

#include "wrappers/VkwImage.h"
#include "wrappers/VkwWorker.h"
#include "wrappers/VkwRandomTexture.h"

namespace world {

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


    /// @param cpu Si true, r�cup�re les textures sur le cpu depuis la carte
    /// graphique
    LodTextures &getOrCreate(int lod, float width, bool cpu = false);

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
    auto &lodTex = _internal->getOrCreate(lod, getWorldWidth(lod));
    return lodTex._layerTextures.at(layer);
}

Image &VkwGroundTextureGenerator::getTexture(int layer, int lod) {
    auto &lodTex = _internal->getOrCreate(lod, getWorldWidth(lod), true);
    return lodTex._layerImages.at(layer);
}

LodTextures &VkwGroundTextureGeneratorPrivate::getOrCreate(int lod, float width,
                                                           bool cpu) {
    auto it = _lodTextures.insert({lod, LodTextures{}});
    LodTextures &t = it.first->second;

    if (it.second) {
        t._width = width;
        launchTextureGeneration(t);
        t._texWorker->waitForCompletion();
    }

    // TODO transitions to textures ?

    if (cpu && t._layerTextures.size() != t._layerImages.size()) {
        t._layerImages.clear();

        for (VkwImage &img : t._layerTextures) {
            t._layerImages.push_back(VkwMemoryHelper::GPUToImage(img));
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

float VkwGroundTextureGenerator::getWorldWidth(int lod) {
    return _baseWorldWidth * powi(2.0f, -lod);
}
} // namespace world
