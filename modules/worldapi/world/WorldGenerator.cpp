//
// Created by louis on 20/04/17.
//

#include "WorldGenerator.h"

#include "FlatWorldGenerator.h"
#include "Environment2DGenerator.h"
#include "MapGenerator.h"
#include "../maths/mathshelper.h"

using namespace maths;

class PrivateWorldGenerator {
public:
	std::vector<std::unique_ptr<WorldGenNode>> _nodes;
};


WorldGenerator* WorldGenerator::createDefaultGenerator() {
	FlatWorldGenerator * result = new FlatWorldGenerator();
	
	PerlinTerrainGenerator * terrain = new PerlinTerrainGenerator(129, 0, 5, 4);

	MapGenerator * map = new MapGenerator(100, 100);
	map->emplaceReliefMapGenerator<CustomWorldRMGenerator>(map);

	result->addPrimaryNode(new Environment2DGenerator(terrain, map));

    return result;
}

WorldGenerator::WorldGenerator()
        : _internal(new PrivateWorldGenerator()) {

}

WorldGenerator::~WorldGenerator() {

}

void WorldGenerator::addPrimaryNode(WorldGenNode * node) {
	_nodes().emplace_back(node);
}

World* WorldGenerator::generate() {
	World * world = new World();
    return world;
}

void WorldGenerator::processGeneration(World & world) {
	for (auto & node : _nodes()) {
		node->startGeneration(world);
	}
}

std::vector<std::unique_ptr<WorldGenNode>> & WorldGenerator::_nodes() {
	return _internal->_nodes;
}