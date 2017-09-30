#pragma once

#include <worldapi/worldapidef.h>

#include <vector>
#include <random>
#include <memory>

#include "Map.h"
#include "WorldGenerator.h"
#include "WorldParameters.h"

// MODULES (à déplacer ?)

class MapGenerator;

/** Classe de base des modules du générateur de cartes. */
class WORLDAPI_EXPORT MapGeneratorModule {
public:
	MapGeneratorModule(MapGenerator * parent);
	virtual ~MapGeneratorModule() = default;

	virtual void generate(Map & map) const = 0;

	virtual MapGeneratorModule * clone(MapGenerator * newParent) = 0;
protected:
	MapGenerator * _parent;

	std::mt19937 & rng() const;
	arma::cube & reliefMap(Map & map) const;
};

/** Classe de base des modules du générateur de cartes servant
à générer la carte de relief. */
class WORLDAPI_EXPORT ReliefMapGenerator : public MapGeneratorModule {
public:
	ReliefMapGenerator(MapGenerator * parent);

	virtual ReliefMapGenerator * clone(MapGenerator * newParent) override = 0;
};

class WORLDAPI_EXPORT CustomWorldRMGenerator : public ReliefMapGenerator {
public:
	CustomWorldRMGenerator(MapGenerator * parent, float biomeDensity = 1, uint32_t limitBrightness = 4);
	CustomWorldRMGenerator(const CustomWorldRMGenerator & other, MapGenerator * newParent);

	void setBiomeDensity(float biomeDensity);
	void setLimitBrightness(uint32_t);

	void setDifferentialLaw(const relief::diff_law & law);

	void generate(Map & map) const override;
	CustomWorldRMGenerator * clone(MapGenerator * newParent) override;
private:
	// la largeur d'un carré unité.
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


// MapGenerator

/** */
class WORLDAPI_EXPORT MapGenerator {
public:
	MapGenerator(uint32_t sizeX, uint32_t sizeY);
	MapGenerator(const MapGenerator& other);
	~MapGenerator();

	Map * generate();

	template <class T, typename... Args>
	T & createReliefMapGenerator(Args&&... args) {
		T * reliefMap = new T(this, args...);
		_reliefMap = std::unique_ptr<T>(reliefMap);
		return *reliefMap;
	}

	MapGenerator * clone() const;
private:
	mutable std::mt19937 _rng;

	uint32_t _sizeX, _sizeY;

	std::unique_ptr<ReliefMapGenerator> _reliefMap;

	friend class MapGeneratorModule;
};

