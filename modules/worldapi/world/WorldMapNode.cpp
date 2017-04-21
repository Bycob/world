//
// Created by louis on 21/04/17.
//

#include "WorldMapNode.h"

INIT_TYPE(WorldMapNode, "worldmap")

WorldMapNode::WorldMapNode(const World * world)
        : WorldNode(world, TYPE(WorldMapNode)) {

}