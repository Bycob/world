#include "WorldGenerator.h"

WorldGenNode::WorldGenNode(WorldGenerator * generator)
        : _parent(generator) {

}

void WorldGenNode::expand(World & world, const maths::vec3d & location) {
	// Default implementation
}

void WorldGenNode::addRequiredNodes(World &world) const {
	// Default implementation
}
