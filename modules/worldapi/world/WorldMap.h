#pragma once

#include <worldapi/worldapidef.h>

#include <stdint.h>
#include <armadillo/armadillo>

#include "../Image.h"

/** G�nerer une worldmap est la premi�re �tape pour ensuite cr�er un monde
entier. La worldmap d�finit les diff�rents param�tres physiques qu'il existera
dans le monde, comme les zones de montagnes, le climat, le type d'�cosyst�me,
et ce en chaque endroit du monde.*/
class WORLDAPI_EXPORT WorldMap {
public:
	WorldMap(uint32_t sizeX, uint32_t sizeY);
	~WorldMap();

	const arma::cube & getReliefMap();
	img::Image getReliefMapAsImage();
private:
	uint32_t sizeX, sizeY;

	/** La carte des reliefs permet de caract�riser d'une part l'altitude
	moyenne et d'autre part l'�tendue du relief, ce en chaque endroit.
	Par cons�quent c'est la carte des reliefs qui d�finit les zones de
	montagnes, les plateaux, les plaines... */
	arma::cube _reliefMap;
};

