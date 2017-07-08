#pragma once

#include "terrain.h"

class TerrainSubdivisionTree {
public:
	TerrainSubdivisionTree(const Terrain & terrain);
	TerrainSubdivisionTree(Terrain && terrain);
	~TerrainSubdivisionTree();

	Terrain & terrain() {
		return *_terrain;
	}

	double getZ(double x, double y, int stage = 0) const;

	double getZInterpolated(double x, double y, int stage = 0) const;

	// ------ Gestion des subdivision

	/** @returns L'étape de subdivision du terrain, c'est à dire de
	combien de subdivision il a été issu.*/
	int getSubdivisionStage() const {
		return _subdivisionStage;
	}

	/** @returns L'écart de niveau de subdivision entre ce noeud et le noeud
	passé en paramètres. */
	int getLevelDiff(const TerrainSubdivisionTree & terrain) const;

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
	const TerrainSubdivisionTree & getSubtreeAt(double x, double y, int stage = 1) const;

	TerrainSubdivisionTree & getSubtree(int xindex, int yindex) const;
private:
	TerrainSubdivisionTree(Terrain * terrain) 
		: _terrain(std::unique_ptr<Terrain>(terrain)) { }

	std::unique_ptr<Terrain> _terrain;

	/** Indique l'étape de subdivision du terrain, c'est-à-dire le nombre
	de subdivisions dont il a été issu.*/
	int _subdivisionStage = 0;
	/** Chaque sous-terrain est composé d'une part de bruit, le reste étant
	une copie interpolée des valeurs du terrain à l'étage en dessus. Ce
	champ indique quelle est la part de bruit dans le terrain et permet ainsi
	d'extraire ce bruit au besoin. */
	double _noisePart = 0;
	/** Si ce terrain est une subdivision, indique la position de cette
	subdivision au sein du terrain parent. */
	int _x, _y;
	/** Noeud parent */
	TerrainSubdivisionTree *_parent = nullptr;

	/** Indique le nombre de subdivision que possède ce terrain en largeur
	et en hauteur. */
	int _subdivideFactor = 0;
	/** Vector contenant tous les sous-terrains. Les terrains sont
	ordonnées en column-major order, c'est à dire que le sous-terrain
	d'indice (x, y) est stocké à la case x * _subdivideFactor + y */
	std::vector<std::unique_ptr<TerrainSubdivisionTree>> _subterrain;

	// ------
	friend class TerrainGenerator;
	friend class PerlinTerrainGenerator;

	/** Avec child le terrain passé en paramètres, enfant du nième degré, et root le terrain
	parent le plus haut dans sa hierarchie, on peut obtenir la position sur child à partir
	de la position sur root passée en paramètres.
	Les coordonnées s'exprime en pourcentage des dimensions des terrains.*/
	void getLocalPosition(const TerrainSubdivisionTree & child, double globalPosX, double globalPosY, double & localPosX, double & localPosY) const;
};

