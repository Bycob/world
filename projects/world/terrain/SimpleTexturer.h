#ifndef WORLD_SIMPLETEXTURING_H
#define WORLD_SIMPLETEXTURING_H

#include "world/core/WorldConfig.h"

#include <random>

#include "world/core/ColorMap.h"
#include "ITerrainWorker.h"

namespace world {

class WORLDAPI_EXPORT SimpleTexturer : public ITerrainWorker {
public:
    SimpleTexturer();

    ColorMap &getColorMap();

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

private:
    std::mt19937 _rng;
    ColorMap _colorMap;
};
} // namespace world

#endif // WORLD_SIMPLETEXTURING_H
