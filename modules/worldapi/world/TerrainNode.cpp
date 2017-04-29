//
// Created by louis on 21/04/17.
//

#include "TerrainNode.h"

#include "Map.h"

INIT_TYPE(TerrainNode, "terrain")

TerrainNode::TerrainNode(const World * world)
        : WorldNode(world, TYPE(TerrainNode)) {

}

TerrainGenNode::TerrainGenNode(WorldGenerator &parent)
        : WorldGenNode(parent) {

}

void TerrainGenNode::addRequiredNodes(World &world) const {
    requireUnique<TerrainNode>(world);
    requireUnique<MapNode>(world);
}