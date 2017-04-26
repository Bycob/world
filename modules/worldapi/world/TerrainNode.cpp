//
// Created by louis on 21/04/17.
//

#include "TerrainNode.h"

INIT_TYPE(TerrainNode, "terrain")

TerrainNode::TerrainNode(const World * world)
        : WorldNode(world, TYPE(TerrainNode)) {

}