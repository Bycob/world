#pragma once

#include "world/core/WorldConfig.h"

#include "Terrain.h"
#include "ITerrainWorker.h"

namespace world {

class WORLDAPI_EXPORT TerrainOps {
public:
    static void fill(Terrain &terrain, double value);

    static void applyOffset(Terrain &terrain, const arma::mat &offset);

    static void applyOffset(Terrain &terrain, double offset);

    static void multiply(Terrain &terrain, const arma::mat &factor);

    static void multiply(Terrain &terrain, double factor);

    static void copyNeighbours(Terrain &terrain, const TileCoordinates &coords,
                               const TerrainGrid &storage);

    /** Find the min value and the max value of this terrain.
     * \returns vec2d{min value, max value} */
    static vec2d computeZBounds(const Terrain &terrain);
};
} // namespace world
