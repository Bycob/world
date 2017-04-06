#pragma once

#include <worldapi\worldapidef.h>

#include <vector>
#include <random>
#include <memory>

#include "WorldMap.h"
#include "../GenBase.h"

// MODULES (à déplacer ?)

class WorldMapGenerator;

/** Classe de base des modules du générateur de cartes. */
class WORLDAPI_EXPORT WorldMapGeneratorModule {
public:
	WorldMapGeneratorModule(WorldMapGenerator * parent);

	virtual void generate(WorldMap & map) const = 0;
protected:
	WorldMapGenerator * _parent;

	std::mt19937 & rng();
	arma::cube & reliefMap(WorldMap & map);
};

/** */
class WORLDAPI_EXPORT ReliefMapGenerator : public WorldMapGeneratorModule {
public:
	ReliefMapGenerator(WorldMapGenerator * parent);

	virtual void generate(WorldMap & map) const;
private:
};


// WorldMapGenerator

/** */
class WORLDAPI_EXPORT WorldMapGenerator : public GenBase<WorldMap> {
public:
	WorldMapGenerator(uint32_t sizeX, uint32_t sizeY);
	~WorldMapGenerator();

	std::unique_ptr<WorldMap> generate();
private:
	mutable std::mt19937 _rng;

	uint32_t _sizeX, _sizeY;

	ReliefMapGenerator _reliefMap;


	friend class WorldMapGeneratorModule;
};