#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <stdint.h>
#include <armadillo/armadillo>

#include "../Image.h"
#include "World.h"

/** Génerer une worldmap est la première étape pour ensuite créer un monde
entier. La worldmap définit les différents paramètres physiques qu'il existera
dans le monde, comme les zones de montagnes, le climat, le type d'écosystème,
et ce en chaque endroit du monde.*/
class WORLDAPI_EXPORT Map {
public:
	Map(uint32_t sizeX, uint32_t sizeY);
	~Map();

	const arma::cube & getReliefMap();
	img::Image getReliefMapAsImage();
private:
	uint32_t _sizeX, _sizeY;

	/** La carte des reliefs permet de caractériser d'une part l'altitude
	moyenne et d'autre part l'étendue du relief, ce en chaque endroit.
	Par conséquent c'est la carte des reliefs qui définit les zones de
	montagnes, les plateaux, les plaines...
	Première tranche : altitude moyenne. Deuxième tranche : étendue.*/
	arma::cube _reliefMap;

	friend class MapGenerator;
	friend class MapGeneratorModule;
};

class WORLDAPI_EXPORT MapNode : public WorldNode {
public:
	DECL_TYPE

	MapNode(const World * world);
private:
	std::unique_ptr<Map> _map;
};


