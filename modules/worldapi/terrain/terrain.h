#pragma once

#include <worldapi/worldapidef.h>

#include <armadillo/armadillo>
#include <vector>
#include <memory>

#include "../mesh.h"
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
est simple couche ou multi couche. 
Cette classe permet également de diviser les terrains en plusieurs morceaux avec des
niveaux de détail plus élevés. On peut ensuite choisir le niveau de détail qu'on veut
observer. */
class WORLDAPI_EXPORT Terrain {

public :
	Terrain(int size);

	Terrain(const arma::Mat<double> & data);

	Terrain(const Terrain & terrain);

	Terrain(Terrain && terrain);

	virtual ~Terrain();

	/** Permet d'obtenir l'altitude au point (x, y), avec x et y les
	coordonnées en pourcentage des dimensions totales du terrain.
	x et y sont compris entre 0 et 1.
	Le résultat n'est pas interpolé.*/
	double getZ(double x, double y, int stage = 0) const;

	/** Même chose que #getZ sauf que cette fois le résultat est interpolé*/
	double getZInterpolated(double x, double y, int stage = 0) const;

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
	const Terrain & getSubterrainAt(double x, double y, int stage = 1) const;

	Terrain & getSubterrain(int xindex, int yindex) const;

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

	arma::Mat<double> _array;
	std::unique_ptr<img::Image> _texture;

	/** Indique l'étape de subdivision du terrain, c'est-à-dire le nombre
	de subdivisions dont il a été issu.*/
	int _subdivisionStage = 0;
	/** Chaque sous-terrain est composé d'une part de bruit, le reste étant
	une copie interpolée des valeurs du terrain à l'étage en dessus. Ce
	champ indique quelle est la part de bruit dans le terrain et permet ainsi
	d'extraire ce bruit au besoin. */
	float _noisePart = 0;
	/** Si ce terrain est une subdivision, indique la position de cette
	subdivision au sein du terrain parent. */
	int _x, _y;
	Terrain *_parent = nullptr;

	/** Indique le nombre de subdivision que possède ce terrain en largeur
	et en hauteur. */
	int _subdivideFactor = 0;
	/** Vector contenant tous les sous-terrains. Les terrains sont
	ordonnées en column-major order, c'est à dire que le sous-terrain
	d'indice (x, y) est stocké à la case x * _subdivideFactor + y */
	std::vector<std::unique_ptr<Terrain>> _subterrain;

	// ------

	friend class TerrainGenerator;
	friend class PerlinTerrainGenerator;

	/** Avec child le terrain passé en paramètres, enfant du nième degré, et root le terrain
	parent le plus haut dans sa hierarchie, on peut obtenir la position sur child à partir
	de la position sur root passée en paramètres.
	Les coordonnées s'exprime en pourcentage des dimensions des terrains.*/
	void getLocalPosition(const Terrain & child, double globalPosX, double globalPosY, double & localPosX, double & localPosY) const;
};
