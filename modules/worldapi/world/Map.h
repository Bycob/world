#pragma once

#include <worldapi/worldapidef.h>

#include <utility>
#include <memory>
#include <stdint.h>
#include <armadillo/armadillo>

#include "../Image.h"
#include "World.h"

/** Génerer une worldmap est la première étape pour ensuite créer un monde
entier. La worldmap définit les différents paramètres physiques qu'il existera
dans le monde, comme les zones de montagnes, le climat, le type d'écosystème,
et ce en chaque endroit du monde.*/
class WORLDAPI_EXPORT Map : public WorldNode {
public:
	DECL_WORLD_NODE_TYPE

	Map(const World * parent = nullptr, uint32_t sizeX = 100, uint32_t sizeY = 100, float unitsPerPixel = 500);
	~Map();

	float getUnitsPerPixel() const;

	const arma::cube & getReliefMap();
	std::pair<double, double> getReliefAt(double x, double y) const;
	img::Image getReliefMapAsImage();
private:
	uint32_t _sizeX, _sizeY;
	/** Indique combien d'unité du monde pour un terrain */
	float _unitsPerPixel;

	/** La carte des reliefs permet de caractériser d'une part l'altitude
	moyenne et d'autre part l'étendue du relief, ce en chaque endroit.
	Par conséquent c'est la carte des reliefs qui définit les zones de
	montagnes, les plateaux, les plaines...
	Première tranche : altitude moyenne. Deuxième tranche : étendue.*/
	arma::cube _reliefMap;

	friend class MapGenerator;
	friend class MapGeneratorModule;
};
