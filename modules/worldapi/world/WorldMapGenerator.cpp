#include "WorldMapGenerator.h"

#include <utility>
#include <list>
#include <tuple>

#include "../maths/mathshelper.h"

using namespace arma;
using namespace maths;
using namespace img;

// MODULES (� d�placer ?)

WorldMapGeneratorModule::WorldMapGeneratorModule(WorldMapGenerator * parent) : 
	_parent(parent) {

}

cube & WorldMapGeneratorModule::reliefMap(WorldMap & map) const {
	return map._reliefMap;
}

std::mt19937 & WorldMapGeneratorModule::rng() const {
	return _parent->_rng;
}


// -----
ReliefMapGenerator::ReliefMapGenerator(WorldMapGenerator * parent) : 
	WorldMapGeneratorModule(parent) {

}


// -----
const float CustomWorldRMGenerator::PIXEL_UNIT = 100;

CustomWorldRMGenerator::CustomWorldRMGenerator(WorldMapGenerator * parent) : 
	ReliefMapGenerator(parent) {
	
}

void CustomWorldRMGenerator::generate(WorldMap & map) const {
	cube relief = reliefMap(map);

	// Nombre de biomes � g�n�rer.
	uint32_t size = relief.n_rows * relief.n_cols;
	uint32_t biomeCount = (uint32_t)(_biomeDensity * (float) size / PIXEL_UNIT);


	// -> Cr�ation de la grille pour le placement des points de mani�re al�atoire,
	// mais avec une distance minimum.

	// Calcul des dimensions de la grille
	float minDistance = PIXEL_UNIT / 2.0 * sqrt(_biomeDensity);
	
	uint32_t sliceCount = (uint32_t)((float)relief.n_rows / minDistance);
	float sliceSize = (float)relief.n_rows / (float)sliceCount;

	uint32_t caseCount = (uint32_t)((float)relief.n_cols / minDistance);
	float caseSize = (float)relief.n_cols / (float)caseCount;

	// Pr�paration de la grille
	typedef std::tuple<float, float, float, float> point; // pour plus de lisibilit�
	std::vector<std::vector<point>> pointsMap; pointsMap.reserve(sliceCount);
	std::vector<std::pair<uint32_t, uint32_t>> grid; grid.reserve(size);

	for (int x = 0; x < sliceCount; x++) {
		pointsMap.emplace_back();

		auto slice = pointsMap[x];
		slice.reserve(caseCount);

		for (int y = 0; y < caseCount; y++) {
			grid.emplace_back(x, y);
			slice.emplace_back(-1, -1, 0, 0);
		}
	}
	
	// G�n�ration des points
	std::uniform_real_distribution<double> rand(0.0, 1.0);

	for (int i = 0; i < biomeCount; i++) {
		// G�n�ration des coordonn�es des points
		uint32_t randIndex = (uint32_t)(rand(rng()) * grid.size());
		std::pair<uint32_t, uint32_t> randPoint = grid.at(randIndex);
		grid.erase(grid.begin() + randIndex);

		uint32_t x = randPoint.first;
		uint32_t y = randPoint.second;

		// Calcul des limites dans lesquelles on peut avoir un point
		double limNegX = 0;
		double limPosX = sliceSize;
		double limNegY = 0;
		double limPosY = caseSize;

		if (x > 0) {
			auto negXCase = pointsMap[x - 1][y];

			if (std::get<0>(negXCase) > 0) {
				limNegX = std::get<0>(negXCase);
			}
		}
		if (x < sliceCount - 1) {
			auto posXCase = pointsMap[x + 1][y];

			if (std::get<0>(posXCase) > 0) {
				limPosX = std::get<0>(posXCase);
			}
		}
		if (y > 0) {
			auto negYCase = pointsMap[x][y - 1];

			if (std::get<1>(negYCase) > 0) {
				limNegY = std::get<1>(negYCase);
			}
		}
		if (y < caseCount - 1) {
			auto posYCase = pointsMap[x][y + 1];

			if (std::get<1>(posYCase) > 0) {
				limPosY = std::get<1>(posYCase);
			}
		}

		// � partir des limites on peut d�terminer la position random du point
		double randX = rand(rng());
		double randY = rand(rng());

		pointsMap[x][y] = std::make_tuple<float, float, float, float>(
			randX * (limPosX - limNegX) + limNegX + x * sliceSize,
			randY * (limPosY - limNegY) + limNegY + y * caseSize,
			0, 0);

		// ET VOILA !
		// Maintenant est-ce qu'on trouve les valeurs ici, ou est-ce qu'on attend ?
		// Est-ce qu'on s�pare cet algorithme du reste ?
		// Tant de question sans r�ponse...
	}


	// -> Interpolation des valeurs des points pour reconstituer une map

}


// WorldMapGenerator

WorldMapGenerator::WorldMapGenerator(uint32_t sizeX, uint32_t sizeY) : 
	_sizeX(sizeX), _sizeY(sizeY),
	_reliefMap(nullptr) {

}

WorldMapGenerator::~WorldMapGenerator() {

}

std::unique_ptr<WorldMap> WorldMapGenerator::generate() {
	std::unique_ptr<WorldMap> map = std::make_unique<WorldMap>(_sizeX, _sizeY);

	if (_reliefMap != nullptr) _reliefMap->generate(*map);

	return map;
}