#pragma once

#include "core/WorldConfig.h"

#include <memory>
#include <map>
#include <utility>

#include "core/WorldTypes.h"
#include "ITerrainWorker.h"
#include "math/Perlin.h"
#include "Terrain.h"

namespace world {

class WORLDAPI_EXPORT PerlinTerrainGenerator : public ITerrainWorker {
public:
    PerlinTerrainGenerator(int octaveCount = 5, double frequency = 8,
                           double persistence = 0.5);

    void setFrequency(double frequency);

    void setPersistence(double persistence);

    void setOctaveCount(int octaveCount);

    void process(Terrain &terrain) override;

    void process(Terrain &terrain, ITerrainWorkerContext &context) override;

private:
    PerlinInfo _perlinInfo;
    Perlin _perlin;

    void processByNeighbours(Terrain &terrain, ITerrainWorkerContext &context);

    void processByTileCoords(Terrain &terrain, ITerrainWorkerContext &context);
};
} // namespace world
