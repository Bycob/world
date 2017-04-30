#pragma once

#include <worldapi/worldapidef.h>

#include <memory>
#include <stdint.h>
#include <armadillo/armadillo>

#include "../Image.h"
#include "World.h"

/** G�nerer une worldmap est la premi�re �tape pour ensuite cr�er un monde
entier. La worldmap d�finit les diff�rents param�tres physiques qu'il existera
dans le monde, comme les zones de montagnes, le climat, le type d'�cosyst�me,
et ce en chaque endroit du monde.*/
class WORLDAPI_EXPORT Map {
public:
	Map(uint32_t sizeX, uint32_t sizeY);
	~Map();

	const arma::cube & getReliefMap();
	img::Image getReliefMapAsImage();
private:
	uint32_t _sizeX, _sizeY;

	/** La carte des reliefs permet de caract�riser d'une part l'altitude
	moyenne et d'autre part l'�tendue du relief, ce en chaque endroit.
	Par cons�quent c'est la carte des reliefs qui d�finit les zones de
	montagnes, les plateaux, les plaines...
	Premi�re tranche : altitude moyenne. Deuxi�me tranche : �tendue.*/
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


