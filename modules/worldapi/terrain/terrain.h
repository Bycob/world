#pragma once

#include <armadillo/armadillo>
#include <vector>
#include <memory>

#include <worldapi/worldapidef.h>

#include "../mesh.h"
#include "../Image.h"

using namespace arma;

/* NB : Toutes les ressources générées par cette classe utilisent le COLUMN MAJOR ORDER,
	c'est à dire que la première colonne de matrice armadillo correspondra à la première
	ligne d'un fichier image correspondant.
	-> x indique l'indice de la ligne,
	-> y indique l'indice de la colonne.*/

class WORLDAPI_EXPORT TerrainGenerator;

/** Cette classe permet de manipuler des terrains planaires avec plusieurs niveaux
de détails. 
Les terrains définis par cette classe sont définis par des surfaces. A chaque point
(x, y) du plan, correspond une ou plusieurs altitude(s) z(i). 
Cette classe permet également de diviser les terrains en plusieurs morceaux avec des
niveaux de détail plus élevés. On peut ensuite choisir le niveau de détail qu'on veut
observer. */
class WORLDAPI_EXPORT Terrain {

public :
	Terrain(int size);

	virtual ~Terrain();

	/** Permet d'obtenir l'altitude au point (x, y), avec x et y les
	coordonnées en pourcentage des dimensions totales du terrain.
	x et y sont compris entre 0 et 1.
	Le résultat n'est pas interpolé.*/
	double getZ(double x, double y, int stage = 0);

	/** Même chose que #getZ sauf que cette fois le résultat est interpolé*/
	double getZInterpolated(double x, double y, int stage = 0);

	// ------ Gestion des subdivision

	/** @returns L'étape de subdivision du terrain, c'est à dire de
	combien de subdivision il a été issu.*/
	int getSubdivisionStage() const {
		return _subdivisionStage;
	}

	int getLevelDiff(const Terrain & terrain) const;

	bool isSubdivided() const;

	int getSubdivideFactor() const {
		return _subdivideFactor;
	}

	/** Subdivise ce terrain en un certain nombre de sous-terrains, déterminé
	par le paramètre subdivideFactor. */
	void subdivide(int subdivideFactor);

	/** Cette méthode donne le sous-terrain qui contient le point (x, y).
	x et y sont les coordonnées en pourcentage des dimensions totales du
	terrain, comprises entre 0 et 1.
	@param stage Le niveau du terrain dans la hierarchie. Va de 0, qui représente
	ce terrain, à n, avec n le nombre d'étage de subdivisions réalisé depuis ce
	terrain.
	@returns Le sous terrain aux coordonnées indiquées, à l'étage indiqué.
	Si l'étage indiqué est trop grand, la méthode retourne le sous-terrain
	à cet endroit qui a l'étage le plus grand. */
	Terrain & getSubterrainAt(double x, double y, int stage = 1);

	Terrain & getSubterrain(double xindex, double yindex);

	// ------ IO

	//Interfaçage avec les fichiers .obj
	Mesh * convertToMesh(float sizeX = 1, float sizeY = 1, float sizeZ = 0.4) const;

	Mesh * convertToMesh(float offsetX, float offsetY, float offsetZ, float sizeX, float sizeY, float sizeZ) const;
	
	/** Convertit ce terrain en submesh, en indiquant la taille du terrain parent
	au niveau de détail le plus bas, c'est à dire le parent le plus éloigné de ce
	terrain. Le mesh issu de la conversion représentera donc une fraction de ce
	parent éloigné.
	Si le niveau de subdivision de ce terrain est 0, alors cette méthode produit le
	même résultat que la méthode #convertToMesh(float, float, float). */
	Mesh * convertToSubmesh(float rootSizeX = 1, float rootSizeY = 1, float rootSizeZ = 0.4) const;

	//Méthodes pour la conversion du terrain en image.
	img::Image convertToImage();

	img::Image getTexture();

	const img::Image & texture();

private :

	Mat<double> _array;
	std::unique_ptr<img::Image> _texture;

	/// Indique l'étape de subdivision du terrain, c'est-à-dire le nombre
	/// de subdivisions dont il a été issu.
	int _subdivisionStage = 0;
	int _x, _y;
	Terrain *_parent = nullptr;

	int _subdivideFactor;
	/// Vector contenant tous les sous-terrains. Les terrains sont
	/// ordonnées en column-major order, c'est à dire que le sous-terrain
	/// d'indice (x, y) est stocké à la case x * _subdivideFactor + y
	std::vector<std::unique_ptr<Terrain>> _subterrain;

	// ------

	friend class TerrainGenerator;
	friend class PerlinTerrainGenerator;

	void getLocalPosition(Terrain & child, double globalPosX, double globalPosY, double * const localPosX, double * const localPosY);
};
