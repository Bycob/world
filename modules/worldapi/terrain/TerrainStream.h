#ifndef WORLD_TERRAINSTREAM_H
#define WORLD_TERRAINSTREAM_H

#include "Terrain.h"

class TerrainInputStream {
public:
    TerrainInputStream(const Terrain& terrain);

private:
    const Terrain& _terrain;
};


#endif //WORLD_TERRAINSTREAM_H
