#pragma once

#include <worldapi/worldapidef.h>

#include <armadillo/armadillo>
#include <vector>
#include <memory>

#include "../maths/MathsHelper.h"
#include "../maths/BoundingBox.h"
#include "../Mesh.h"
#include "../Image.h"

/* NB : Toutes les ressources générées par cette classe utilisent le COLUMN MAJOR ORDER,
	c'est à dire que la première colonne de matrice armadillo correspondra à la première
	ligne d'un fichier image correspondant.
	-> x indique l'indice de la ligne,
	-> y indique l'indice de la colonne.*/

class WORLDAPI_EXPORT TerrainGenerator;

/** Cette classe permet de manipuler des terrains planaires avec plusieurs niveaux
de détails. 
Les terrains définis par cette classe sont définis par des surfaces. A chaque point
(x, y) du plan, correspond une ou plusieurs altitude(s) z(i), suivant si le terrain
est simple couche ou multi couche. */
class WORLDAPI_EXPORT Terrain {

public :
	Terrain(int size);

	Terrain(const arma::Mat<double> & data);

	Terrain(const Terrain & terrain);
	Terrain(Terrain && terrain);

	virtual ~Terrain();

    uint32_t getSize() const {
        return _array.n_rows;
    }

	double& operator()(int x, int y) {
		return _array(x, y);
	}

	/** Permet d'obtenir l'altitude au point (x, y), avec x et y les
	coordonnées en pourcentage des dimensions totales du terrain.
	x et y sont compris entre 0 et 1.
	Le résultat n'est pas interpolé.*/
	double getZ(double x, double y) const;

	/** Même chose que #getZ sauf que cette fois le résultat est interpolé*/
	double getZInterpolated(double x, double y) const;

	// ------ IO

	//Interfaçage avec les fichiers .obj
    Mesh * convertToMesh();
	Mesh * convertToMesh(double sizeX, double sizeY, double sizeZ) const;
	Mesh * convertToMesh(double offsetX, double offsetY, double offsetZ, double sizeX, double sizeY, double sizeZ) const;
	
	//Méthodes pour la conversion du terrain en image.
	img::Image convertToImage() const;

	img::Image getTexture() const;

	const img::Image & texture() const;

private :

    maths::BoundingBox _bbox;
	arma::Mat<double> _array;
	std::unique_ptr<img::Image> _texture;

	// ------

	friend class TerrainGenerator;
	friend class PerlinTerrainGenerator;
	friend class TerrainManipulator;

	maths::vec2i getPixelPos(double x, double y) const;
};
