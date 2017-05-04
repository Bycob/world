//
// Created by louis on 21/04/17.
//

#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include <worldapi/worldapidef.h>

#include "World.h"
#include "WorldGenerator.h"
#include "../terrain/terrain.h"

/** Cette classe gère le sol du monde. */
class WORLDAPI_EXPORT Ground : public WorldNode {
public:
    DECL_TYPE

    Ground(const World * world);
private:

};

class WORLDAPI_EXPORT GroundGenerator : public WorldGenNode {
public:
    GroundGenerator(WorldGenerator * parent);

    void addRequiredNodes(World & world) const override;
    GroundGenerator * clone(WorldGenerator * newParent) override;
};


#endif //WORLD_TERRAINNODE_H
