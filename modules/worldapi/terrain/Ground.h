//
// Created by louis on 21/04/17.
//

#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include <worldapi/worldapidef.h>

#include <map>
#include <utility>
#include <functional>

#include "Terrain.h"
#include "../world/Chunk.h"
#include "../world/MapGenerator.h"

class FlatWorld;
class GroundCache;

/** Cette classe gère le sol du monde. Le sol est composé de plusieurs
terrains carrés accolés les uns aux autres. Les caractéristiques de ces
terrains dépendent notament d'une Map générée en amont.
Cette classe peut faire référence à un dossier contenant des terrains
déjà générés. Dans ce cas, les terrains sont chargés dynamiquement par
la classe Ground lorsque l'utilisateur le demande. Un terrain donné est
considéré comme non généré s'il n'est ni dans le cache, ni dans le dossier
référencé. */
class WORLDAPI_EXPORT Ground {
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
	const Terrain & getTerrain(int x, int y, int lvl = 0) const;

	std::string getTerrainDataId(int x, int y, int lvl) const;

	const Terrain & getTerrainAt(double x, double y, int lvl = 0) const;
	double getAltitudeAt(double x, double y, int lvl = 0) const;

private:
	// Paramètres
	/** L'altitude maximum du monde. Le niveau de la mer est fixé à 0. */
	double _maxAltitude;
	/** L'altitude minimum du monde. Le niveau de la mer est fixé à 0. */
	double _minAltitude;
	/** La taille d'un terrain de niveau 0 utilisé pour paver la Map. Normalement,
	cette taille est la même que celle d'un pixel de la Map. */
	double _unitSize;
    int _factor = 4;

    // Generator
    std::unique_ptr<ReliefMapGenerator> _mapGenerator;
    std::unique_ptr<TerrainGenerator> _terrainGenerator;

	// Données
	int _cacheSize;
	mutable GroundCache * _cache;

	std::map<maths::vec3i, std::unique_ptr<Terrain>> & terrains() const;
	Terrain & terrain(int x, int y, int lvl = 0);

    double getTerrainSize(int level) const;
    void generateChunk(FlatWorld &world, Chunk &chunk);
    void generateTerrain(int x, int y, int lvl);
    void applyMap(int x, int y, int lvl, bool unapply);
};


#endif //WORLD_TERRAINNODE_H
