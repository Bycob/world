//
// Created by louis on 21/04/17.
//

#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include <worldapi/worldapidef.h>

#include "World.h"
#include "WorldGenerator.h"
#include "../terrain/terrain.h"

class TerrainNode : public WorldNode {
public:
    DECL_TYPE

    TerrainNode(const World * world);
private:

};

class TerrainGenNode : public WorldGenNode {
public:
    TerrainGenNode(WorldGenerator & parent);

    virtual void addRequiredNodes(World & world) const;
};


#endif //WORLD_TERRAINNODE_H
