#ifndef WORLD_TERRAINSTREAM_H
#define WORLD_TERRAINSTREAM_H

#include "core/WorldConfig.h"

#include "Terrain.h"

namespace world {

    class TerrainInputStream {
    public:
        TerrainInputStream(const Terrain &terrain);

    private:
        const Terrain &_terrain;
    };
}

#endif //WORLD_TERRAINSTREAM_H
