#ifndef WORLD_ALTITUDETEXTURING_H
#define WORLD_ALTITUDETEXTURING_H

#include "world/core/WorldConfig.h"

#include <random>

#include "world/core/ColorMap.h"
#include "ITerrainWorker.h"

namespace world {

class WORLDAPI_EXPORT AltitudeTexturer : public ITerrainWorker {
public:
    AltitudeTexturer();

    ColorMap &getColorMap();

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

private:
    std::mt19937 _rng;
    ColorMap _colorMap;
};
} // namespace world

#endif // WORLD_ALTITUDETEXTURING_H
