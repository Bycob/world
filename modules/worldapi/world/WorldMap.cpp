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