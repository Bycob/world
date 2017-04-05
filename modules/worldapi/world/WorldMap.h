#pragma once

#include <worldapi/worldapidef.h>

#include <stdint.h>
#include <armadillo/armadillo>

#include "../Image.h"

/** Génerer une worldmap est la première étape pour ensuite créer un monde
entier. La worldmap définit les différents paramètres physiques qu'il existera
dans le monde, comme les zones de montagnes, le climat, le type d'écosystème,
et ce en chaque endroit du monde.*/
class WORLDAPI_EXPORT WorldMap {
public:
	WorldMap(uint32_t sizeX, uint32_t sizeY);
	~WorldMap();

	const arma::cube & getReliefMap();
	img::Image getReliefMapAsImage();
private:
	uint32_t sizeX, sizeY;

	/** La carte des reliefs permet de caractériser d'une part l'altitude
	moyenne et d'autre part l'étendue du relief, ce en chaque endroit.
	Par conséquent c'est la carte des reliefs qui définit les zones de
	montagnes, les plateaux, les plaines... */
	arma::cube _reliefMap;
};

