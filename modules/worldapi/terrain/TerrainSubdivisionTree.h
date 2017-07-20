#pragma once

#include <worldapi/worldapidef.h>

#include "terrain.h"

// TODO mettre � jour la doc

/** Cette classe permet de diviser des terrains en plusieurs morceaux avec des
niveaux de d�tail plus �lev�s. On peut ensuite choisir le niveau de d�tail qu'on veut
observer.*/
class WORLDAPI_EXPORT TerrainSubdivisionTree {
public:
	TerrainSubdivisionTree(const Terrain & terrain);
	TerrainSubdivisionTree(Terrain && terrain);
	TerrainSubdivisionTree(const TerrainSubdivisionTree & other);
	TerrainSubdivisionTree(TerrainSubdivisionTree && other);
	~TerrainSubdivisionTree();

	Terrain & terrain() {
		return *_terrain;
	}

	const Terrain & terrain() const {
		return *_terrain;
	}

	double getZ(double x, double y, int stage = 0) const;

	double getZInterpolated(double x, double y, int stage = 0) const;

	// ------ Gestion des subdivision

	/** @returns L'�tape de subdivision du terrain, c'est � dire de
	combien de subdivision il a �t� issu.*/
	int getSubdivisionStage() const {
		return _subdivisionStage;
	}

	/** @returns L'�cart de niveau de subdivision entre ce noeud et le noeud
	pass� en param�tres. */
	int getLevelDiff(const TerrainSubdivisionTree & terrain) const;

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
	const TerrainSubdivisionTree & getSubtreeAt(double x, double y, int stage = 1) const;

	TerrainSubdivisionTree & getSubtree(int xindex, int yindex) const;

	/** Convertit ce terrain en submesh, en indiquant la taille du terrain parent
	au niveau de d�tail le plus bas, c'est � dire le parent le plus �loign� de ce
	terrain. Le mesh issu de la conversion repr�sentera donc une fraction de ce
	parent �loign�.
	Si le niveau de subdivision de ce terrain est 0, alors cette m�thode produit le
	m�me r�sultat que la m�thode #convertToMesh(float, float, float). */
	Mesh * convertToSubmesh(float rootSizeX = 1, float rootSizeY = 1, float rootSizeZ = 0.4) const;

private:
	TerrainSubdivisionTree(Terrain * terrain) 
		: _terrain(std::unique_ptr<Terrain>(terrain)) { }

	std::unique_ptr<Terrain> _terrain;

	/** Indique l'�tape de subdivision du terrain, c'est-�-dire le nombre
	de subdivisions dont il a �t� issu.*/
	int _subdivisionStage = 0;
	/** Chaque sous-terrain est compos� d'une part de bruit, le reste �tant
	une copie interpol�e des valeurs du terrain � l'�tage en dessus. Ce
	champ indique quelle est la part de bruit dans le terrain et permet ainsi
	d'extraire ce bruit au besoin. */
	double _noisePart = 0;
	/** Si ce terrain est une subdivision, indique la position de cette
	subdivision au sein du terrain parent. */
	int _x, _y;
	/** Noeud parent */
	TerrainSubdivisionTree *_parent = nullptr;

	/** Indique le nombre de subdivision que poss�de ce terrain en largeur
	et en hauteur. */
	int _subdivideFactor = 0;
	/** Vector contenant tous les sous-terrains. Les terrains sont
	ordonn�es en column-major order, c'est � dire que le sous-terrain
	d'indice (x, y) est stock� � la case x * _subdivideFactor + y */
	std::vector<std::unique_ptr<TerrainSubdivisionTree>> _subtrees;

	// ------
	friend class TerrainGenerator;
	friend class PerlinTerrainGenerator;

	/** Avec child le terrain pass� en param�tres, enfant du ni�me degr�, et root le terrain
	parent le plus haut dans sa hierarchie, on peut obtenir la position sur child � partir
	de la position sur root pass�e en param�tres.
	Les coordonn�es s'exprime en pourcentage des dimensions des terrains.*/
	void getLocalPosition(const TerrainSubdivisionTree & child, double globalPosX, double globalPosY, double & localPosX, double & localPosY) const;
};

