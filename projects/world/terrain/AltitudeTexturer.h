#ifndef WORLD_ALTITUDETEXTURING_H
#define WORLD_ALTITUDETEXTURING_H

#include "world/core/WorldConfig.h"

#include <random>

#include "world/core/ColorMap.h"
#include "ITerrainWorker.h"

namespace world {

class WORLDAPI_EXPORT AltitudeTexturer : public ITerrainWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    AltitudeTexturer();

    ColorMap &getColorMap();

    void processTerrain(Terrain &terrain) override;

    void processTile(ITileContext &context) override;

    void write(WorldFile &wf) const;

    void read(const WorldFile &wf);

private:
    std::mt19937 _rng;
    ColorMap _colorMap;
};
} // namespace world

#endif // WORLD_ALTITUDETEXTURING_H
