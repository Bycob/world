#include "MapGenerator.h"

#include <utility>
#include <list>
#include <tuple>

#include "worldapi/assets/Image.h"
#include "../maths/MathsHelper.h"
#include "../maths/Interpolation.h"

using namespace arma;


namespace world {

	// -----
	ReliefMapGenerator::ReliefMapGenerator() {

	}


	// -----
	const double CustomWorldRMGenerator::PIXEL_UNIT = 10;

	CustomWorldRMGenerator::CustomWorldRMGenerator(double biomeDensity, uint32_t limitBrightness) :
			_biomeDensity(biomeDensity),
			_limitBrightness(limitBrightness),
			_diffLaw(std::make_unique<relief::CustomWorldDifferential>()) {

	}

	CustomWorldRMGenerator::CustomWorldRMGenerator(const CustomWorldRMGenerator &other)
			: _biomeDensity(other._biomeDensity),
			  _limitBrightness(other._limitBrightness),
			  _diffLaw(other._diffLaw->clone()) {

	}

	void CustomWorldRMGenerator::setBiomeDensity(float biomeDensity) {
		_biomeDensity = biomeDensity;
	}

	void CustomWorldRMGenerator::setLimitBrightness(uint32_t p) {
		_limitBrightness = p;
	}

	void CustomWorldRMGenerator::setDifferentialLaw(const relief::diff_law &law) {
		_diffLaw = std::unique_ptr<relief::diff_law>(law.clone());
	}

	void CustomWorldRMGenerator::generate(Terrain &height, Terrain &heightDiff) const {
		// Nombre de biomes à générer.
		uint32_t size = (uint32_t) (height.getSize() * height.getSize());
		uint32_t biomeCount = (uint32_t) (_biomeDensity * (double) size / (PIXEL_UNIT * PIXEL_UNIT));


		// -> Création de la grille pour le placement des points de manière aléatoire,
		// mais avec une distance minimum.

		// Calcul des dimensions de la grille
		double minDistance = PIXEL_UNIT / 2.0 * sqrt(_biomeDensity);

		// TODO unifier
		uint32_t sliceCount = max<uint32_t>((uint32_t) ((float) height.getSize() / minDistance), 1);
		float sliceSize = (float) height.getSize() / (float) sliceCount;

		uint32_t caseCount = max<uint32_t>((uint32_t) ((float) height.getSize() / minDistance), 1);
		float caseSize = (float) height.getSize() / (float) caseCount;

		// Préparation de la grille
		typedef std::pair<vec2d, vec2d> point; // pour plus de lisibilité
		std::vector<std::vector<point>> pointsMap;
		pointsMap.reserve(sliceCount);
		std::vector<std::pair<uint32_t, uint32_t>> grid;
		grid.reserve(sliceCount * caseCount);

		for (int x = 0; x < sliceCount; x++) {
			pointsMap.emplace_back();

			std::vector<point> &slice = pointsMap[x];
			slice.reserve(caseCount);

			for (int y = 0; y < caseCount; y++) {
				grid.emplace_back(x, y);
				slice.emplace_back(vec2d(-1, -1), vec2d(0, 0));
			}
		}

		// Génération des points
		std::uniform_real_distribution<double> rand(0.0, 1.0);

		for (int i = 0; i < biomeCount; i++) { // TODO dans les cas limites la grille peut se vider totalement
			// Génération des coordonnées des points
			uint32_t randIndex = (uint32_t) (rand(_rng) * grid.size());
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

				if (negXCase.first.x >= 0) {
					limNegX = negXCase.first.x;
				}
			}
			if (x < sliceCount - 1) {
				auto posXCase = pointsMap[x + 1][y];

				if (posXCase.first.x >= 0) {
					limPosX = posXCase.first.x;
				}
			}
			if (y > 0) {
				auto negYCase = pointsMap[x][y - 1];

				if (negYCase.first.y >= 0) {
					limNegY = negYCase.first.y;
				}
			}
			if (y < caseCount - 1) {
				auto posYCase = pointsMap[x][y + 1];

				if (posYCase.first.y >= 0) {
					limPosY = posYCase.first.y;
				}
			}

			// à partir des limites on peut déterminer la position random du point
			double randX = rand(_rng);
			double randY = rand(_rng);

			// TODO L'utilisateur n'a aucun contrôle sur le premier paramètre.
			float elevation = rand(_rng);
			float diff = (*_diffLaw)(std::pair<double, double>(elevation, rand(_rng)));

			pointsMap[x][y] = std::make_pair<vec2d, vec2d>(
					vec2d(
							(float) (randX * (limPosX - limNegX) + limNegX + x * sliceSize),
							(float) (randY * (limPosY - limNegY) + limNegY + y * caseSize)),
					vec2d(elevation, diff));
		}


		// -> Interpolation des valeurs des points pour reconstituer une map

		// Création des interpolateur
		IDWInterpolator<vec2d, vec2d> interpolator(_limitBrightness);

		// On prépare les données à interpoler.
		for (auto &slice : pointsMap) {
			for (auto &pt : slice) {
				if (pt.first.x >= 0) {
					interpolator.addData(pt.first, pt.second);
				}
			}
		}

		// On remplit la grille à l'aide de l'interpolateur. And enjoy.
		for (uint32_t x = 0; x < height.getSize(); x++) {
			for (uint32_t y = 0; y < height.getSize(); y++) {
				vec2d pt((double) x + 0.5, (double) y + 0.5);
				vec2d result = interpolator.getData(pt);
				height(x, y) = result.x;
				heightDiff(x, y) = result.y;
			}
		}
	}
}
