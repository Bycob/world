#include "Map.h"

#include <stdexcept>

using namespace arma;
using namespace img;

INIT_TYPE(Map, "map", true)

Map::Map(const World * parent, uint32_t sizeX, uint32_t sizeY, float unitsPerPixel) :
	WorldNode(parent, TYPE(Map)),
	_sizeX(sizeX), _sizeY(sizeY),
	_unitsPerPixel(unitsPerPixel),
	_reliefMap(sizeX, sizeY, 2) {
	
}

Map::~Map() {

}

float Map::getUnitsPerPixel() const {
	return _unitsPerPixel;
}

const cube & Map::getReliefMap() {
	return _reliefMap;
}

Image Map::getReliefMapAsImage() {
	Image img(_reliefMap.n_rows, _reliefMap.n_cols, ImageType::RGB);

	for (uint32_t x = 0; x < _reliefMap.n_rows; x++) {
		for (uint32_t y = 0; y < _reliefMap.n_cols; y++) {
			img.at(x, y).set(
				(uint8_t) (_reliefMap(x, y, 0) * 255),
				(uint8_t) ((1 - _reliefMap(x, y, 1)) * 255)
			);
		}
	}
	return img;
}