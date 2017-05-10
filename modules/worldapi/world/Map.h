#pragma once

#include <worldapi/worldapidef.h>

#include <utility>
#include <memory>
#include <stdint.h>
#include <armadillo/armadillo>

#include "../Image.h"
#include "World.h"

/** G�nerer une worldmap est la premi�re �tape pour ensuite cr�er un monde
entier. La worldmap d�finit les diff�rents param�tres physiques qu'il existera
dans le monde, comme les zones de montagnes, le climat, le type d'�cosyst�me,
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
	/** Indique combien d'unit� du monde pour un terrain */
	float _unitsPerPixel;

	/** La carte des reliefs permet de caract�riser d'une part l'altitude
	moyenne et d'autre part l'�tendue du relief, ce en chaque endroit.
	Par cons�quent c'est la carte des reliefs qui d�finit les zones de
	montagnes, les plateaux, les plaines...
	Premi�re tranche : altitude moyenne. Deuxi�me tranche : �tendue.*/
	arma::cube _reliefMap;

	friend class MapGenerator;
	friend class MapGeneratorModule;
};
