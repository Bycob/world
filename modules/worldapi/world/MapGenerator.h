#pragma once

#include <worldapi/worldapidef.h>

#include <vector>
#include <random>
#include <memory>

#include "../terrain/Terrain.h"
#include "WorldParameters.h"


/** Classe de base des modules du générateur de cartes servant
à générer la carte de relief. */
class WORLDAPI_EXPORT ReliefMapGenerator {
public:
	ReliefMapGenerator();

	virtual void generate(Terrain& height, Terrain& heightDiff) const = 0;
protected:
	mutable std::mt19937 _rng;
};

class WORLDAPI_EXPORT CustomWorldRMGenerator : public ReliefMapGenerator {
public:
	CustomWorldRMGenerator(double biomeDensity = 1, uint32_t limitBrightness = 4);
	CustomWorldRMGenerator(const CustomWorldRMGenerator & other);

	void setBiomeDensity(float biomeDensity);
	void setLimitBrightness(uint32_t);

	void setDifferentialLaw(const relief::diff_law & law);

	void generate(Terrain& height, Terrain& heightDiff) const override;
private:
	// la largeur d'un carré unité.
	static const double PIXEL_UNIT;
	/** Le nombre moyen de biomes par bloc de 100 pixels de WorldMap.*/
	double _biomeDensity;
	/** La netteté des limites entre les biomes. En pratique c'est
	le "p" dans l'algo de l'interpolation. */
	uint32_t _limitBrightness;

	/** Loi de probabilité du différentiel d'altitude en fonction
	de l'altitude. */
	std::unique_ptr<relief::diff_law> _diffLaw;
};
