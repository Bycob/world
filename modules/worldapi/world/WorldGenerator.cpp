//
// Created by louis on 20/04/17.
//

#include "WorldGenerator.h"

class PrivateWorldGenerator {
public:
	std::vector<std::unique_ptr<WorldGenNode>> _nodes;
};


WorldGenerator* WorldGenerator::defaultGenerator() {
    WorldGenerator * result = new WorldGenerator();

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

void WorldGenerator::init(World &world) {

    // Ajout des noeuds requis pour le monde.
    for (std::unique_ptr<WorldGenNode> & node : _internal->_nodes) {
        node->addRequiredNodes(world);
    }
}