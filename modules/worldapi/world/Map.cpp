#include "Map.h"

#include <stdexcept>

#include "../maths/mathshelper.h"
#include "../maths/interpolation.h"

using namespace arma;
using namespace img;
using namespace maths;

Map::Map(uint32_t sizeX, uint32_t sizeY, float unitsPerPixel) :
	_sizeX(sizeX), _sizeY(sizeY),
	_unitsPerPixel(unitsPerPixel),
	_reliefMap(sizeX, sizeY, 2) {

	_reliefMap.slice(0).fill(0);
	_reliefMap.slice(1).fill(1);
}

Map::~Map() {

}

void Map::setUnitsPerPixel(float units) {
	_unitsPerPixel = units;
}

const cube & Map::getReliefMap() const {
	return _reliefMap;
}

std::pair<double, double> Map::getReliefAt(double x, double y) const {
	// Vérification des paramètres
	x = clamp(x, 0, _sizeX - 2 - std::numeric_limits<double>::epsilon());
	y = clamp(y, 0, _sizeY - 2 - std::numeric_limits<double>::epsilon());

	int xi = (int) floor(x);
	int yi = (int) floor(y);
	double xd = x - xi;
	double yd = y - yi;
	
	double offset1 = interpolateLinear(0, _reliefMap(xi, yi, 0), 1, _reliefMap(xi + 1, yi, 0), xd);
	double offset2 = interpolateLinear(0, _reliefMap(xi, yi + 1, 0), 1, _reliefMap(xi + 1, yi + 1, 0), xd);
	double offset = interpolateLinear(0, offset1, 1, offset2, yd);

	double diff1 = interpolateLinear(0, _reliefMap(xi, yi, 1), 1, _reliefMap(xi + 1, yi, 1), xd);
	double diff2 = interpolateLinear(0, _reliefMap(xi, yi + 1, 1), 1, _reliefMap(xi + 1, yi + 1, 1), xd);
	double diff = interpolateLinear(0, diff1, 1, diff2, yd);

	return std::pair<double, double>(offset, diff);
}

Image Map::getReliefMapAsImage() const {
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