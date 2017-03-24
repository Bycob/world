#include "TerrainGenerator.h"
#include "../maths/perlin.h"
#include "../Image.h"

using namespace img;
using namespace perlin;

TerrainGenerator::TerrainGenerator(int size) : _size(size) {

}

TerrainGenerator::~TerrainGenerator() {

}

void TerrainGenerator::setSubdivisionsCount(int subdivisionCount) {
	_subdivisionCount = subdivisionCount;
}

void TerrainGenerator::setSize(int size) {
	_size = size;
}

void TerrainGenerator::generateSubdivisions(Terrain & terrain, int subdivideFactor, int subdivisionsCount) {
	terrain.subdivide(subdivideFactor);
	
	for (int x = 0; x < subdivideFactor; x++) {
		for (int y = 0; y < subdivideFactor; y++) {
			generateSubdivision(terrain, x, y);
		}
	}
	//TODO harmonisation horizontale entre les différentes subdivisions.

	// Subdivisions à l'étage au dessus
	if (subdivisionsCount != 1) {
		for (int x = 0; x < subdivideFactor; x++) {
			for (int y = 0; y < subdivideFactor; y++) {
				generateSubdivisions(terrain.getSubterrain(x, y), subdivideFactor, subdivisionsCount - 1);
			}
		}
	}
}

//TODO check des tailles des tableaux passés en paramètres (map et randomArray)

Image TerrainGenerator::generateTexture(const Terrain & terrain, const TerrainTexmapBuilder & builder, const Mat<double> & randomArray) const {
	//TODO support de l'altitude
	auto map = builder.convertToMap();

	return generateTexture(terrain, map, randomArray);
}

//Cette méthode là ne supportera pas l'altitude.
Image TerrainGenerator::generateTexture(const Terrain & terrain, const arma::Cube<double> & map, const Mat<double> & randomArray) const {
	float scaleX = (float)terrain._array.n_rows / randomArray.n_rows;
	float scaleY = (float)terrain._array.n_cols / randomArray.n_cols;
	arma::Cube<double> result(randomArray.n_rows, randomArray.n_cols, 3);

	for (int x = 0; x < randomArray.n_rows; x++) {
		for (int y = 0; y < randomArray.n_cols; y++) {
			double x1 = (double) x / randomArray.n_rows;
			double y1 = (double) y / randomArray.n_cols;

			double z = terrain.getZInterpolated(x1, y1);
			double randParam = randomArray(x, y);

			result(x, y, 0) = map((int)(z * (RESOLUTION - 1)), (int)(randParam * (RESOLUTION - 1)), 0);
			result(x, y, 1) = map((int)(z * (RESOLUTION - 1)), (int)(randParam * (RESOLUTION - 1)), 1);
			result(x, y, 2) = map((int)(z * (RESOLUTION - 1)), (int)(randParam * (RESOLUTION - 1)), 2);
		}
	}

	return Image(result);
}



PerlinTerrainGenerator::PerlinTerrainGenerator(int size, int offset, int octaveCount, double frequency, double persistence) : 
	TerrainGenerator(size) , _offset(offset), _octaveCount(octaveCount), _frequency(frequency), _persistence(persistence) {

}

PerlinTerrainGenerator::~PerlinTerrainGenerator() {
	
}

std::unique_ptr<Terrain> PerlinTerrainGenerator::generate() const {
	std::unique_ptr<Terrain> result = std::make_unique<Terrain>(_size);
	generatePerlinNoise2D(result->_array, _offset, _octaveCount, _frequency, _persistence);

	return std::move(result);
}

void PerlinTerrainGenerator::generateSubdivision(Terrain & terrain, int xsub, int ysub) const {
	//TODO Faire plusieurs tests pour voir si on obtient un meilleur résultat en changeant les paramètres.
	Terrain & subterrain = terrain.getSubterrain(xsub, ysub);
	generatePerlinNoise2D(subterrain._array, _offset, _octaveCount, _frequency, _persistence);

	double oneTerrainLength = 1.0 / terrain._subdivideFactor;

	//on interpole avec les valeurs de l'étage du dessus.
	for (int x = 0; x < subterrain._array.n_rows; x++) {
		for (int y = 0; y < subterrain._array.n_cols; y++) {
			subterrain._array(x, y) -= 0.5;
			subterrain._array(x, y) *= _subdivNoiseRatio;
			subterrain._array(x, y) += terrain.getZInterpolated(
				((double) xsub + (double) x / subterrain._array.n_rows) / terrain._subdivideFactor,
				((double) ysub + (double) y / subterrain._array.n_rows) / terrain._subdivideFactor, 0);
		}
	}
}
