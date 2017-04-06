#include "WorldMapGenerator.h"

#include "../maths/mathshelper.h"

using namespace arma;
using namespace maths;
using namespace img;

// MODULES (à déplacer ?)

WorldMapGeneratorModule::WorldMapGeneratorModule(WorldMapGenerator * parent) : 
	_parent(parent) {

}

cube & WorldMapGeneratorModule::reliefMap(WorldMap & map) {
	return map._reliefMap;
}

std::mt19937 & WorldMapGeneratorModule::rng() {
	return _parent->_rng;
}


// -----
ReliefMapGenerator::ReliefMapGenerator(WorldMapGenerator * parent) : 
	WorldMapGeneratorModule(parent) {

}

void ReliefMapGenerator::generate(WorldMap & map) const {

}


// WorldMapGenerator

WorldMapGenerator::WorldMapGenerator(uint32_t sizeX, uint32_t sizeY) : 
	_sizeX(sizeX), _sizeY(sizeY), _reliefMap(this) {

}

WorldMapGenerator::~WorldMapGenerator() {

}

std::unique_ptr<WorldMap> WorldMapGenerator::generate() {
	std::unique_ptr<WorldMap> map = std::make_unique<WorldMap>(_sizeX, _sizeY);

	_reliefMap.generate(*map);

	return map;
}