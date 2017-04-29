#include "WorldGenerator.h"

WorldGenNode::WorldGenNode(WorldGenerator &generator)
        : _parent(&generator) {

}

void WorldGenNode::addRequiredNodes(World &world) const {

}
