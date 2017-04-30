//
// Created by louis on 21/04/17.
//

#include "Ground.h"

#include "Map.h"

INIT_TYPE(Ground, "ground")

Ground::Ground(const World * world)
        : WorldNode(world, TYPE(Ground)) {

}



GroundGenerator::GroundGenerator(WorldGenerator * parent)
        : WorldGenNode(parent) {

}

void GroundGenerator::addRequiredNodes(World &world) const {
	requireUnique<Map>(world);
    requireUnique<Ground>(world);
}