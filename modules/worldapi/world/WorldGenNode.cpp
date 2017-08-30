#include "WorldGenerator.h"

void WorldGenNode::startGeneration(World & world) {
	generate(world);
	generateChildren(world);
}

void WorldGenNode::addNode(WorldGenNode * node) {
	_childrens.push_back(node);
}

void WorldGenNode::generateChildren(World & world) {
	for (auto node : _childrens) {
		node->generate(world);
	}
}