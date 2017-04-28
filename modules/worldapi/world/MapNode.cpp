//
// Created by louis on 21/04/17.
//

#include "Map.h"

INIT_TYPE(MapNode, "map")

MapNode::MapNode(const World * world)
        : WorldNode(world, TYPE(MapNode)) {

}