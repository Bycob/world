#include "FlatWorldGenerator.h"

void FlatWorldGenNode::startGeneration(FlatWorld & world) {
	WorldGenNode::startGeneration(world);

	generate(world);
	generateChildren(world);
}

void FlatWorldGenNode::generateChildren(FlatWorld & world) {
	for (auto node : _children) {
		node->startGeneration(world);
	}
}

void FlatWorldGenNode::addFlatWorldNode(FlatWorldGenNode * node) {
	_children.push_back(node);
}


class PrivateFlatWorldGenerator {
public:
	std::vector<std::unique_ptr<FlatWorldGenNode>> _nodes;
};



FlatWorldGenerator::FlatWorldGenerator() : _internal(new PrivateFlatWorldGenerator()) {

}

FlatWorldGenerator::~FlatWorldGenerator() {
	delete _internal;
}

void FlatWorldGenerator::addPrimaryNode(FlatWorldGenNode * node) {
	_internal->_nodes.emplace_back(node);
}

World * FlatWorldGenerator::generate() {
	return generateFlatWorld();
}

FlatWorld * FlatWorldGenerator::generateFlatWorld() {
	FlatWorld * result = new FlatWorld();

	WorldGenerator::processGeneration(*result);
	processFlatWorldGeneration(*result);

	return result;
}

void FlatWorldGenerator::processFlatWorldGeneration(FlatWorld & world) {
	for (auto & node : _internal->_nodes) {
		node->startGeneration(world);
	}
}