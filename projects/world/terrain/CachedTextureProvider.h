#ifndef WORLD_CACHEDTEXTUREPROVIDER_H
#define WORLD_CACHEDTEXTUREPROVIDER_H

#include "world/core/WorldConfig.h"

#include <map>
#include <utility>

#include "MultilayerGroundTexture.h"

namespace world {

/** Provide textures from files in the specified directory. Textures
 * have usually been written by other texture providers. This class
 * allows to reuse cached texture even if the provider that produced
 * them is no more available. */
class WORLDAPI_EXPORT CachedTextureProvider : public ITextureProvider {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    CachedTextureProvider(std::string path = "");

    Image &getTexture(int layer, int lod) override;

private:
    std::map<std::pair<int, int>, Image> _images;
};

} // namespace world

#endif //WORLD_CACHEDTEXTUREPROVIDER_H
