#ifndef WORLD_DEFAULTTEXTUREPROVIDER_H
#define WORLD_DEFAULTTEXTUREPROVIDER_H

#include "world/core/WorldConfig.h"

#include <map>
#include <utility>

#include "MultilayerGroundTexture.h"

namespace world {

class WORLDAPI_EXPORT DefaultTextureProvider : public ITextureProvider {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    DefaultTextureProvider(std::string path = "");

    Image &getTexture(int layer, int lod) override;

private:
    std::map<std::pair<int, int>, Image> _images;
};

} // namespace world

#endif // WORLD_DEFAULTTEXTUREPROVIDER_H
