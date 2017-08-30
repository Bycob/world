//
// Created by louis on 21/04/17.
//

#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include <worldapi/worldapidef.h>

#include <map>
#include <utility>

#include "../world/WorldObject.h"
#include "../world/IPointOfView.h"
#include "terrain.h"

class GroundCache;

struct TerrainTile {
	Terrain * _terrain;
	int _x;
	int _y;
};

/** Cette classe gère le sol du monde. Le sol est composé de plusieurs
terrains carrés accolés les uns aux autres. Les caractéristiques de ces
terrains dépendent notament d'une Map générée en amont.
Cette classe peut faire référence à un dossier contenant des terrains
déjà générés. Dans ce cas, les terrains sont chargés dynamiquement par
la classe Ground lorsque l'utilisateur le demande. Un terrain donné est
considéré comme non généré s'il n'est ni dans le cache, ni dans le dossier
référencé. */
class WORLDAPI_EXPORT Ground : public WorldObject {
public:
    Ground();
	virtual ~Ground();

	// TODO constraints
	void setMaxAltitude(float max) { _maxAltitude = max; }
	void setMinAltitude(float min) { _minAltitude = min; }
	void setUnitSize(float unitSize) { _unitSize = unitSize; }

	float getMaxAltitude() const { return _maxAltitude; }
	float getMinAltitude() const { return _minAltitude; }
	float getAltitudeRange() const { return _maxAltitude - _minAltitude; }
	float getUnitSize() const { return _unitSize; }
	
	/** Indique si le terrain à l'indice (x, y) existe déjà ou au
	contraire doit être généré par le générateur de terrain.
	@returns true si le terrain existe, false s'il doit être généré.*/
	bool isTerrainGenerated(int x, int y, int lvl = 0) const;
	/** Donne le terrain d'indice (x, y). */
	const Terrain & getTerrain(int x, int y) const;

	std::string getTerrainDataId(int x, int y, int lvl) const;

	const Terrain & getTerrainAt(double x, double y, int lvl = 0) const;

	const std::vector<TerrainTile> getTerrainsFrom(const IPointOfView & pointOfView) const;
private:
	// Paramètres
	/** L'altitude maximum du monde. Le niveau de la mer est fixé à 0. */
	float _maxAltitude;
	/** L'altitude minimum du monde. Le niveau de la mer est fixé à 0. */
	float _minAltitude;
	/** La taille d'un terrain de niveau 0 utilisé pour paver la Map. Normalement,
	cette taille est la même que celle d'un pixel de la Map. */
	float _unitSize;

	// Données
	int _cacheSize;
	mutable GroundCache * _cache;

	std::map<std::pair<int, int>, std::unique_ptr<Terrain>> & terrains() const;
	Terrain & terrain(int x, int y);

	friend class Environment2DGenerator; // TODO temporaire, trouver une solution plus propre
};


#endif //WORLD_TERRAINNODE_H
