//
// Created by louis on 21/04/17.
//

#ifndef WORLD_WORLDMAPNODE_H
#define WORLD_WORLDMAPNODE_H

#include <worldapi/worldapidef.h>

#include <memory>

#include "WorldNode.h"
#include "WorldMap.h"

class WorldMapNode : public WorldNode {
public:
    DECL_TYPE

    WorldMapNode(const World * world);
private:
    std::unique_ptr<WorldMap> _map;
};


#endif //WORLD_WORLDMAPNODE_H
