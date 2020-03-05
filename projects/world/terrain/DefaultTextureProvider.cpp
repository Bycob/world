#include "DefaultTextureProvider.h"

namespace world {

DefaultTextureProvider::DefaultTextureProvider(std::string path) {
    if (!path.empty()) {
        _cache.setRoot(path);
    }
}

Image &DefaultTextureProvider::getTexture(int layer, int lod) {
    std::pair<int, int> key(layer, lod);
    auto it = _images.find(key);

    if (it == _images.end()) {
        // Lookup cache (crash if image does not exists)
        std::string imgId = getImageId(layer, lod);
        it = _images.insert({key, _cache.readImage(imgId)}).first;
    }

    return it->second;
}

} // namespace world
