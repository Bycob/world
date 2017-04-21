//
// Created by louis on 21/04/17.
//

#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include <worldapi/worldapidef.h>

#include "../world/WorldNode.h"
#include "terrain.h"

class TerrainNode : public WorldNode {
public:
    static WorldNodeType type;

    TerrainNode();
private:

};


#endif //WORLD_TERRAINNODE_H
