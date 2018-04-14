#pragma once

#include "core/WorldConfig.h"

#include <vector>
#include <map>
#include <random>
#include <memory>

#include "ITerrainWorker.h"
#include "ReliefParameters.h"

namespace world {

	/** Base class for generating relief maps.
	 * A relief map is compound of 2 layers : an "offset" layer
	 * and a "diff" layer. The offset is the minimum/average
	 * altitude at each point, and the diff quantify the variation
	 * of altitude.
	 * For example, mountains have a high offset and a high diff,
	 * countryside has a low offset and a low diff, and a plateau
	 * would have a high offset but a low diff. */
	class WORLDAPI_EXPORT ReliefMapModifier : public ITerrainWorker {
	public:
		ReliefMapModifier();

		void setMapResolution(int mapres);

		void process(Terrain &terrain) override;

		void process(Terrain &terrain, ITerrainWorkerContext &context) override;

		const std::pair<Terrain, Terrain> &obtainMap(int x, int y);
	protected:
        int _mapResolution = 200;
		double _mapPointSize = 2000;
		mutable std::mt19937 _rng;
		std::map<vec2i, std::pair<Terrain, Terrain>> _reliefMap;

		virtual void generate(Terrain &height, Terrain &heightDiff) = 0;
	};

	class WORLDAPI_EXPORT CustomWorldRMModifier : public ReliefMapModifier {
	public:
		CustomWorldRMModifier(double biomeDensity = 1, int limitBrightness = 4);

		void setBiomeDensity(float biomeDensity);

		void setLimitBrightness(int);

		void setDifferentialLaw(const diff_law &law);

	protected:
		void generate(Terrain &height, Terrain &heightDiff) override;

	private:
		// la largeur d'un carré unité.
		static const double PIXEL_UNIT;
		/** Le nombre moyen de biomes par bloc de 100 pixels de WorldMap.*/
		double _biomeDensity;
		/** La netteté des limites entre les biomes. En pratique c'est
        le "p" dans l'algo de l'interpolation. */
		int _limitBrightness;

		/** Loi de probabilité du différentiel d'altitude en fonction
        de l'altitude. */
		diff_law _diffLaw;
	};
}
