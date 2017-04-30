//
// Created by louis on 20/04/17.
//

#include "WorldGenerator.h"

#include "MapGenerator.h"
#include "Ground.h"

class PrivateWorldGenerator {
public:
	std::vector<std::unique_ptr<WorldGenNode>> _nodes;
};


WorldGenerator* WorldGenerator::defaultGenerator() {
    WorldGenerator * result = new WorldGenerator();

	result->_nodes().emplace_back(new MapGenerator(100, 100, result));
	result->_nodes().emplace_back(new GroundGenerator(result));

    return result;
}

WorldGenerator::WorldGenerator() {

}

WorldGenerator::WorldGenerator(const WorldGenerator &other) {

}

WorldGenerator::~WorldGenerator() {

}

World* WorldGenerator::generate() {
    return new World(*this);
}

std::vector<std::unique_ptr<WorldGenNode>> & WorldGenerator::_nodes() {
	return _internal->_nodes;
}

void WorldGenerator::init(World &world) {

    // Ajout des noeuds requis pour le monde.
    for (std::unique_ptr<WorldGenNode> & node : _internal->_nodes) {
        node->addRequiredNodes(world);
    }
}