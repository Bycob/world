#include "GroundTextureGenerator.h"

#include "world/assets/ImageUtils.h"

namespace world {

    WORLD_REGISTER_CHILD_CLASS(ITextureProvider, GroundTextureGenerator, "GroundTextureGenerator")

    GroundTextureGenerator::GroundTextureGenerator(u32 texWidth)
        : _texWidth(texWidth) {};

    Image &GroundTextureGenerator::getTexture(int layer, int lod) {
        // Get from already generated
        auto it = _images.find({layer, lod});

        if (it != _images.end()) {
            return it->second;
        }

        // Get from cache
        std::string imageId = getImageId(layer, lod);

        if (_cache.hasImage(imageId)) {
             auto in = _images.insert({
                 {layer, lod},
                 _cache.readImage(imageId)
                 });
             return in.first->second;
        }

        // Generate image if possible
        if (layer >= _colors.size())
            throw std::runtime_error("Layer index " + std::to_string(layer)
                + " is out of range (max = " + std::to_string(_colors.size()) + ")");

        auto in = _images.insert({
            {layer, lod},
            Image(_texWidth, _texWidth, ImageType::RGBA)
        });
        Image &img = in.first->second;
        ImageUtils::fill(img, _colors[layer]);
        _cache.saveImage(imageId, img);
        return img;
    }

    void GroundTextureGenerator::addLayer(const Color4d &color) {
        _colors.push_back(color);
    }

    void GroundTextureGenerator::addLayer(const BiomeLayer &layer) {
        addLayer(layer._colors.at(0));
    }

    void GroundTextureGenerator::write(WorldFile &wf) const {
        ITextureProvider::write(wf);
        wf.addUint("texWidth", _texWidth);
        wf.addArray("colors");

        for (auto &color: _colors) {
            wf.addToArray("colors", world::serialize(color));
        }
    }

    void GroundTextureGenerator::read(const WorldFile &wf) {
        ITextureProvider::read(wf);
        wf.readUintOpt("texWidth", _texWidth);

        for (auto it = wf.readArray("colors"); !it.end(); ++it) {
            _colors.push_back(world::deserialize<Color4d>(*it));
        }
    }
}