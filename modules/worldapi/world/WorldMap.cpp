#include "WorldMap.h"

#include <stdexcept>

using namespace arma;
using namespace img;

WorldMap::WorldMap(uint32_t sizeX, uint32_t sizeY) : 
	_sizeX(sizeX), _sizeY(sizeY),
	_reliefMap(sizeX, sizeY, 2) {
	
}

WorldMap::~WorldMap() {

}

const cube & WorldMap::getReliefMap() {
	return _reliefMap;
}

Image WorldMap::getReliefMapAsImage() {
	throw std::runtime_error("Not yet implemented");
}