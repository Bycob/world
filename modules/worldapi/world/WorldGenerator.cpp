//
// Created by louis on 20/04/17.
//

#include "WorldGenerator.h"

#include "MapGenerator.h"
#include "Ground.h"
#include "../maths/mathshelper.h"

using namespace maths;

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

WorldGenerator::WorldGenerator()
        : _internal(new PrivateWorldGenerator()),
	      _expandRadius(3000) {

}

WorldGenerator::WorldGenerator(const WorldGenerator &other)
		: _internal(new PrivateWorldGenerator()),
		  _expandRadius(other._expandRadius) {

    for (auto & node : other._internal->_nodes) {
        _internal->_nodes.emplace_back(node->clone(this));
    }
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

void WorldGenerator::expand(World & world, const vec3d &location) {
	for (std::unique_ptr<WorldGenNode> & node : _internal->_nodes) {
		node->expand(world, location);
	}
}