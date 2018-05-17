#ifndef WORLD_ALTITUDETEXTURING_H
#define WORLD_ALTITUDETEXTURING_H

#include "core/WorldConfig.h"

#include <random>

#include "core/ColorMap.h"
#include "ITerrainWorker.h"

namespace world {

class WORLDAPI_EXPORT AltitudeTexturer : public ITerrainWorker {
public:
    AltitudeTexturer(int pixelPerVertex = 4);

    ColorMap &getColorMap();

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

private:
    int _pixelPerVertex;

    std::mt19937 _rng;
    ColorMap _colorMap;
};
} // namespace world

#endif // WORLD_ALTITUDETEXTURING_H
