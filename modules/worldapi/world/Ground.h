//
// Created by louis on 21/04/17.
//

#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include <worldapi/worldapidef.h>

#include <map>

#include "World.h"
#include "WorldGenerator.h"
#include "../terrain/terrain.h"
#include "../terrain/TerrainGenerator.h"
#include "../maths/Vector.h"

/** Cette classe gère le sol du monde. Le sol est composé de plusieurs
terrains carrés accolés les uns aux autres. Les caractéristiques de ces
terrains dépendent notament d'une Map générée en amont.
Cette classe peut faire référence à un dossier contenant des terrains
déjà générés. Dans ce cas, les terrains sont chargés dynamiquement par
la classe Ground lorsque l'utilisateur le demande. Un terrain donné est
considéré comme non généré s'il n'est ni dans le cache, ni dans le dossier
référencé. */
class WORLDAPI_EXPORT Ground : public WorldNode {
public:
    DECL_TYPE

    Ground(const World * world);

	void setHeight(float height);
	void setSeaLevel(float seaLevel);
	
	/** Indique si le terrain à l'indice (x, y) existe déjà ou au
	contraire doit être généré par le générateur de terrain.
	@returns true si le terrain existe, false s'il doit être généré.*/
	bool isTerrainGenerated(int x, int y, int lvl = 0) const;
	/** Donne le terrain d'indice (x, y). */
	const Terrain & getTerrain(int x, int y) const;
private:
	// Paramètres
	/** La hauteur maximum du décor. */
	float _height;
	/** Le niveau de la mer, dans la même unité que _height. */
	float _seaLevel;
	/** La taille d'un terrain de niveau 0 utilisé pour paver la Map. Normalement,
	cette taille est la même que celle d'un pixel de la Map. */
	float _unitSize;

	// Données
	mutable std::map<maths::vec2i, Terrain> _terrains;

	Terrain & getTerrain(int x, int y);

	friend class GroundGenerator;
};

class WORLDAPI_EXPORT GroundGenerator : public WorldGenNode {
public:
    GroundGenerator(WorldGenerator * parent);
	GroundGenerator(WorldGenerator * newParent, const GroundGenerator & other);

	void expand(World & world, const maths::vec3d &location) override;

    void addRequiredNodes(World & world) const override;
    GroundGenerator * clone(WorldGenerator * newParent) override;

private:
	std::unique_ptr<TerrainGenerator> _generator;

	/** nombre de subdivisions effectuées pour les différents niveaux
	de détails du terrain. */
	uint32_t _subdivisions;
};


#endif //WORLD_TERRAINNODE_H
