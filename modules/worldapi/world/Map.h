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

	/** Donne les caract�ristiques du relief � un endroit donn�.
	 * Les coordonn�es sont exprim�es en pixel, le point (0, 0)
	 * correspondant � un coin de la carte.
	 * @param x L'abscisse du point �tudi�.
	 * @param y L'ordonn�e du point �tudi�.
	 * @returns une paire contenant deux informations sur le relief
	 * au point (x, y) : l'altitude et l'�tendue de d'altitude.
	 * @see _reliefMap*/
	std::pair<double, double> getReliefAt(double x, double y) const;

	/** Construit une image RGB qui repr�sente la carte des reliefs. Le
	 * relief est compos� d'une composante d'altitude et d'une composante
	 * d'�tendue d'altitude. Pour le repr�senter, deux canaux de l'image
	 * sont utilis�s.
	 * @see _reliefMap */
	img::Image getReliefMapAsImage();
private:
	uint32_t _sizeX, _sizeY;
	/** Indique combien d'unit� du monde pour un pixel de la map */
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
