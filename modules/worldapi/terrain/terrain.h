#pragma once

#include <armadillo/armadillo>
#include <vector>
#include <memory>

#include <worldapi/worldapidef.h>

#include "../mesh.h"
#include "../Image.h"

using namespace arma;

/* NB : Toutes les ressources g�n�r�es par cette classe utilisent le COLUMN MAJOR ORDER,
	c'est � dire que la premi�re colonne de matrice armadillo correspondra � la premi�re
	ligne d'un fichier image correspondant.
	-> x indique l'indice de la ligne,
	-> y indique l'indice de la colonne.*/

class WORLDAPI_EXPORT TerrainGenerator;

/** Cette classe permet de manipuler des terrains planaires avec plusieurs niveaux
de d�tails. 
Les terrains d�finis par cette classe sont d�finis par des surfaces. A chaque point
(x, y) du plan, correspond une ou plusieurs altitude(s) z(i). 
Cette classe permet �galement de diviser les terrains en plusieurs morceaux avec des
niveaux de d�tail plus �lev�s. On peut ensuite choisir le niveau de d�tail qu'on veut
observer. */
class WORLDAPI_EXPORT Terrain {

public :
	Terrain(int size);

	virtual ~Terrain();

	/** Permet d'obtenir l'altitude au point (x, y), avec x et y les
	coordonn�es en pourcentage des dimensions totales du terrain.
	x et y sont compris entre 0 et 1.
	Le r�sultat n'est pas interpol�.*/
	double getZ(double x, double y, int stage = 0);

	/** M�me chose que #getZ sauf que cette fois le r�sultat est interpol�*/
	double getZInterpolated(double x, double y, int stage = 0);

	// ------ Gestion des subdivision

	/** @returns L'�tape de subdivision du terrain, c'est � dire de
	combien de subdivision il a �t� issu.*/
	int getSubdivisionStage() const {
		return _subdivisionStage;
	}

	int getLevelDiff(const Terrain & terrain) const;

	bool isSubdivided() const;

	int getSubdivideFactor() const {
		return _subdivideFactor;
	}

	/** Subdivise ce terrain en un certain nombre de sous-terrains, d�termin�
	par le param�tre subdivideFactor. */
	void subdivide(int subdivideFactor);

	/** Cette m�thode donne le sous-terrain qui contient le point (x, y).
	x et y sont les coordonn�es en pourcentage des dimensions totales du
	terrain, comprises entre 0 et 1.
	@param stage Le niveau du terrain dans la hierarchie. Va de 0, qui repr�sente
	ce terrain, � n, avec n le nombre d'�tage de subdivisions r�alis� depuis ce
	terrain.
	@returns Le sous terrain aux coordonn�es indiqu�es, � l'�tage indiqu�.
	Si l'�tage indiqu� est trop grand, la m�thode retourne le sous-terrain
	� cet endroit qui a l'�tage le plus grand. */
	Terrain & getSubterrainAt(double x, double y, int stage = 1);

	Terrain & getSubterrain(double xindex, double yindex);

	// ------ IO

	//Interfa�age avec les fichiers .obj
	Mesh * convertToMesh(float sizeX = 1, float sizeY = 1, float sizeZ = 0.4) const;

	Mesh * convertToMesh(float offsetX, float offsetY, float offsetZ, float sizeX, float sizeY, float sizeZ) const;
	
	/** Convertit ce terrain en submesh, en indiquant la taille du terrain parent
	au niveau de d�tail le plus bas, c'est � dire le parent le plus �loign� de ce
	terrain. Le mesh issu de la conversion repr�sentera donc une fraction de ce
	parent �loign�.
	Si le niveau de subdivision de ce terrain est 0, alors cette m�thode produit le
	m�me r�sultat que la m�thode #convertToMesh(float, float, float). */
	Mesh * convertToSubmesh(float rootSizeX = 1, float rootSizeY = 1, float rootSizeZ = 0.4) const;

	//M�thodes pour la conversion du terrain en image.
	img::Image convertToImage();

	img::Image getTexture();

	const img::Image & texture();

private :

	Mat<double> _array;
	std::unique_ptr<img::Image> _texture;

	/// Indique l'�tape de subdivision du terrain, c'est-�-dire le nombre
	/// de subdivisions dont il a �t� issu.
	int _subdivisionStage = 0;
	int _x, _y;
	Terrain *_parent = nullptr;

	int _subdivideFactor;
	/// Vector contenant tous les sous-terrains. Les terrains sont
	/// ordonn�es en column-major order, c'est � dire que le sous-terrain
	/// d'indice (x, y) est stock� � la case x * _subdivideFactor + y
	std::vector<std::unique_ptr<Terrain>> _subterrain;

	// ------

	friend class TerrainGenerator;
	friend class PerlinTerrainGenerator;

	void getLocalPosition(Terrain & child, double globalPosX, double globalPosY, double * const localPosX, double * const localPosY);
};
