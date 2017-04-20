#pragma once

#include <worldapi/worldapidef.h>

#include <vector>
#include <random>
#include <memory>

#include "WorldMap.h"
#include "WorldParameters.h"
#include "../GenBase.h"

// MODULES (à déplacer ?)

class WorldMapGenerator;

/** Classe de base des modules du générateur de cartes. */
class WORLDAPI_EXPORT WorldMapGeneratorModule {
public:
	WorldMapGeneratorModule(WorldMapGenerator * parent);
	virtual ~WorldMapGeneratorModule() = default;

	virtual void generate(WorldMap & map) const = 0;
protected:
	WorldMapGenerator * _parent;

	std::mt19937 & rng() const;
	arma::cube & reliefMap(WorldMap & map) const;
};

/** Classe de base des modules du générateur de cartes servant
à générer la carte de relief. */
class WORLDAPI_EXPORT ReliefMapGenerator : public WorldMapGeneratorModule {
public:
	ReliefMapGenerator(WorldMapGenerator * parent);
};

class WORLDAPI_EXPORT CustomWorldRMGenerator : public ReliefMapGenerator {
public:
	CustomWorldRMGenerator(WorldMapGenerator * parent, float biomeDensity = 1, uint32_t limitBrightness = 4);

	void setBiomeDensity(float biomeDensity);
	void setLimitBrightness(uint32_t);

	void setDifferentialLaw(const relief::diff_law & law);

	virtual void generate(WorldMap & map) const;
private:
	static const float PIXEL_UNIT;
	/** Le nombre moyen de biomes par bloc de 100 pixels de WorldMap.*/
	float _biomeDensity;
	/** La netteté des limites entre les biomes. En pratique c'est
	le "p" dans l'algo de l'interpolation. */
	uint32_t _limitBrightness;

	/** Loi de probabilité du différentiel d'altitude en fonction
	de l'altitude. */
	std::unique_ptr<relief::diff_law> _diffLaw;
};


// WorldMapGenerator

/** */
class WORLDAPI_EXPORT WorldMapGenerator : public GenBase<WorldMap> {
public:
	WorldMapGenerator(uint32_t sizeX, uint32_t sizeY);
	~WorldMapGenerator();

	std::unique_ptr<WorldMap> generate();

	template <class T, typename... Args>
	void emplaceReliefMapGenerator(Args&&... args) {
		_reliefMap = std::make_unique<T>(this, args...);
	}
private:
	mutable std::mt19937 _rng;

	uint32_t _sizeX, _sizeY;

	std::unique_ptr<ReliefMapGenerator> _reliefMap;


	friend class WorldMapGeneratorModule;
};
