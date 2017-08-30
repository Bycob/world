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
class WORLDAPI_EXPORT Map {
public:

	Map(uint32_t sizeX = 100, uint32_t sizeY = 100, float unitsPerPixel = 2000);
	~Map();

	int getSizeX() const {
		return _sizeX;
	}
	int getSizeY() const {
		return _sizeY;
	}
	float getUnitsPerPixel() const {
		return _unitsPerPixel;
	}

	const arma::cube & getReliefMap() const;

	/** Donne les caractéristiques du relief à un endroit donné.
	 * Les coordonnées sont exprimées en pixel, le point (0, 0)
	 * correspondant à un coin de la carte.
	 * @param x L'abscisse du point étudié.
	 * @param y L'ordonnée du point étudié.
	 * @returns une paire contenant deux informations sur le relief
	 * au point (x, y) : l'altitude et l'étendue de d'altitude.
	 * @see _reliefMap*/
	std::pair<double, double> getReliefAt(double x, double y) const;

	/** Construit une image RGB qui représente la carte des reliefs. Le
	 * relief est composé d'une composante d'altitude et d'une composante
	 * d'étendue d'altitude. Pour le représenter, deux canaux de l'image
	 * sont utilisés.
	 * @see _reliefMap */
	img::Image getReliefMapAsImage();
private:
	uint32_t _sizeX, _sizeY;
	/** Indique combien d'unité du monde pour un pixel de la map */
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
