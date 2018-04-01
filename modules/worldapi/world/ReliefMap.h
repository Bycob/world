#pragma once

#include <worldapi/worldapidef.h>

#include <vector>
#include <random>
#include <memory>

#include "../terrain/Terrain.h"
#include "WorldParameters.h"

namespace world {

	/** Base class for generating relief maps.
	 * A relief map is compound of 2 layers : an "offset" layer
	 * and a "diff" layer. The offset is the minimum/average
	 * altitude at each point, and the diff quantify the variation
	 * of altitude.
	 * For example, mountains have a high offset and a high diff,
	 * countryside has a low offset and a low diff, and a plateau
	 * would have a high offset but a low diff. */
	class WORLDAPI_EXPORT ReliefMapGenerator {
	public:
		ReliefMapGenerator();

		virtual void generate(Terrain &height, Terrain &heightDiff) const = 0;

	protected:
		mutable std::mt19937 _rng;
	};

	class WORLDAPI_EXPORT CustomWorldRMGenerator : public ReliefMapGenerator {
	public:
		CustomWorldRMGenerator(double biomeDensity = 1, int limitBrightness = 4);

		CustomWorldRMGenerator(const CustomWorldRMGenerator &other);

		void setBiomeDensity(float biomeDensity);

		void setLimitBrightness(int);

		void setDifferentialLaw(const relief::diff_law &law);

		void generate(Terrain &height, Terrain &heightDiff) const override;

	private:
		// la largeur d'un carr� unit�.
		static const double PIXEL_UNIT;
		/** Le nombre moyen de biomes par bloc de 100 pixels de WorldMap.*/
		double _biomeDensity;
		/** La nettet� des limites entre les biomes. En pratique c'est
        le "p" dans l'algo de l'interpolation. */
		int _limitBrightness;

		/** Loi de probabilit� du diff�rentiel d'altitude en fonction
        de l'altitude. */
		std::unique_ptr<relief::diff_law> _diffLaw;
	};
}
