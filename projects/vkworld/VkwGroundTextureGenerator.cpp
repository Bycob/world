#include "VkwGroundTextureGenerator.h"

#include <memory>
#include <vector>
#include <unordered_map>

#include "world/assets/Shader.h"

#include "wrappers/VkwImage.h"
#include "wrappers/VkwWorker.h"
#include "wrappers/VkwRandomTexture.h"
#include "wrappers/VkwMemoryHelper.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITextureProvider, VkwGroundTextureGenerator,
                           "VkwGroundTextureGenerator")

struct LodTextures {
    float _width = 0;
    std::unique_ptr<VkwGraphicsWorker> _texWorker;
    std::unordered_map<int, std::unique_ptr<VkwImage>> _layerTextures;
    std::unordered_map<int, std::unique_ptr<Image>> _layerImages;
};

class VkwGroundTextureGeneratorPrivate {
public:
    u32 _texWidth = 1024;
    std::vector<Shader> _layers;
    VkwRandomTexture _randTex;

    /// Generated textures
    std::map<int, LodTextures> _lodTextures;


    void launchTextureGeneration(LodTextures &t,
                                 const std::vector<int> &layerIds);
};

VkwGroundTextureGenerator::VkwGroundTextureGenerator()
        : _internal(new VkwGroundTextureGeneratorPrivate()) {}

VkwGroundTextureGenerator::~VkwGroundTextureGenerator() { delete _internal; }


void VkwGroundTextureGenerator::addLayer(const std::string &textureShader) {
    _internal->_layers.emplace_back("generic-texture.vert", textureShader);
}

void VkwGroundTextureGenerator::addLayer(Shader shader) {
    _internal->_layers.push_back(std::move(shader));
}

void VkwGroundTextureGenerator::addLayer(const BiomeLayer &layer) {
    _internal->_layers.emplace_back("generic-texture.vert", layer._shader);
    Shader &shader = _internal->_layers.back();

    for (size_t i = 0; i < layer._colors.size(); ++i) {
        auto &col = layer._colors[i];
        shader.addParameter(
            {ShaderParam::Type::SCALAR, "color" + std::to_string(i),
             std::to_string(col._r) + "," + std::to_string(col._g) + "," +
                 std::to_string(col._b)});
    }
}

size_t VkwGroundTextureGenerator::getLayerCount() {
    return _internal->_layers.size();
}

VkwImage &VkwGroundTextureGenerator::getVkTexture(int layer, int lod) {
    auto &lodTex = getOrCreate(layer, lod, false);
    return *lodTex._layerTextures.at(layer);
}

Image &VkwGroundTextureGenerator::getTexture(int layer, int lod) {
    auto &lodTex = getOrCreate(layer, lod, true);
    return *lodTex._layerImages.at(layer);
}

void VkwGroundTextureGenerator::write(WorldFile &wf) const {
    ITextureProvider::write(wf);
    wf.addUint("texWidth", _internal->_texWidth);
    wf.addArray("layers");

    for (auto &layer : _internal->_layers) {
        wf.addToArray("layers", layer.serialize());
    }
}

void VkwGroundTextureGenerator::read(const WorldFile &wf) {
    ITextureProvider::read(wf);
    wf.readUintOpt("texWidth", _internal->_texWidth);

    for (auto it = wf.readArray("layers"); !it.end(); ++it) {
        _internal->_layers.emplace_back("generic-texture.vert", "");
        _internal->_layers.back().read(*it);
    }
}

LodTextures &VkwGroundTextureGenerator::getOrCreate(int layerId, int lod,
                                                    bool cpu) {
    auto it = _internal->_lodTextures.insert({lod, LodTextures{}});
    LodTextures &t = it.first->second;

    if (it.second) {
        t._width = getWidth(lod);
    }

    std::string imageId = getImageId(layerId, lod);
    bool generatedCpu = t._layerImages.find(layerId) != t._layerImages.end();
    bool generatedGpu =
        t._layerTextures.find(layerId) != t._layerTextures.end();

    if ((cpu && !generatedCpu) || (!cpu && !generatedGpu)) {
        if (_cache.hasImage(imageId)) {
            t._layerImages[layerId] =
                std::make_unique<Image>(_cache.readImage(imageId));

            if (!cpu) {
                Image &img = *t._layerImages[layerId];
                t._layerTextures[layerId] = std::make_unique<VkwImage>(
                    img.width(), img.height(), VkwImageUsage::OFFSCREEN_RENDER);
                VkwMemoryHelper::imageToGPU(img, *t._layerTextures[layerId]);
            }
        } else {
            if (!generatedGpu) {
                _internal->launchTextureGeneration(t, {layerId});
                t._texWorker->waitForCompletion();

                // TODO transitions to textures ?
            }

            if (cpu || _cache.isAvailable()) {
                t._layerImages[layerId] = std::make_unique<Image>(
                    VkwMemoryHelper::GPUToImage(*t._layerTextures.at(layerId)));
                _cache.saveImage(imageId, *t._layerImages[layerId]);
            }
        }
    }

    return t;
}

void VkwGroundTextureGeneratorPrivate::launchTextureGeneration(
    LodTextures &t, const std::vector<int> &layerIds) {
    // TODO if not enough parameters, segfault

    auto &ctx = Vulkan::context();
    t._texWorker = std::make_unique<VkwGraphicsWorker>();

    for (int layerId : layerIds) {
        auto &shader = _layers.at(layerId);
        auto &image =
            *(t._layerTextures[layerId] = std::make_unique<VkwImage>(
                  _texWidth, _texWidth, VkwImageUsage::OFFSCREEN_RENDER));

        // Generate image
        VkwDescriptorSetLayout layout({0}, {});
        layout.addBinding(DescriptorType::IMAGE, 1);

        int id = 2;
        for (auto &param : shader.getParameters()) {
            layout.addBinding(DescriptorType::UNIFORM_BUFFER, id);
            ++id;
        }

        VkwGraphicsPipeline pipeline(layout);
        pipeline.enableVertexBuffer(false);

        // Setup shader
        pipeline.setBuiltinShader(VkwShaderType::VERTEX,
                                  shader.getVertexPath());
        pipeline.setBuiltinShader(VkwShaderType::FRAGMENT,
                                  shader.getFragmentPath());

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

        id = 2;
        for (auto &param : shader.getParameters()) {
            u8 *data = new u8[ShaderParam::MAX_SIZE];
            int size = 0;
            param.getData(data, size);
            VkwSubBuffer buf = ctx.allocate(
                size, DescriptorType::UNIFORM_BUFFER, MemoryUsage::CPU_WRITES);
            buf.setData(data);

            dset.addDescriptor(id, buf);
            ++id;

            delete[] data;
        }

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
