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

/** Cette classe g�re le sol du monde. Le sol est compos� de plusieurs
terrains carr�s accol�s les uns aux autres. Les caract�ristiques de ces
terrains d�pendent notament d'une Map g�n�r�e en amont.
Cette classe peut faire r�f�rence � un dossier contenant des terrains
d�j� g�n�r�s. Dans ce cas, les terrains sont charg�s dynamiquement par
la classe Ground lorsque l'utilisateur le demande. Un terrain donn� est
consid�r� comme non g�n�r� s'il n'est ni dans le cache, ni dans le dossier
r�f�renc�. */
class WORLDAPI_EXPORT Ground : public WorldNode {
public:
    DECL_TYPE

    Ground(const World * world);

	void setHeight(float height);
	void setSeaLevel(float seaLevel);
	
	/** Indique si le terrain � l'indice (x, y) existe d�j� ou au
	contraire doit �tre g�n�r� par le g�n�rateur de terrain.
	@returns true si le terrain existe, false s'il doit �tre g�n�r�.*/
	bool isTerrainGenerated(int x, int y, int lvl = 0) const;
	/** Donne le terrain d'indice (x, y). */
	const Terrain & getTerrain(int x, int y) const;
private:
	// Param�tres
	/** La hauteur maximum du d�cor. */
	float _height;
	/** Le niveau de la mer, dans la m�me unit� que _height. */
	float _seaLevel;
	/** La taille d'un terrain de niveau 0 utilis� pour paver la Map. Normalement,
	cette taille est la m�me que celle d'un pixel de la Map. */
	float _unitSize;

	// Donn�es
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

	/** nombre de subdivisions effectu�es pour les diff�rents niveaux
	de d�tails du terrain. */
	uint32_t _subdivisions;
};


#endif //WORLD_TERRAINNODE_H
