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

/** Cette classe g�re le sol du monde. Le sol est compos� de plusieurs
terrains carr�s accol�s les uns aux autres. Les caract�ristiques de ces
terrains d�pendent notament d'une Map g�n�r�e en amont.
Cette classe peut faire r�f�rence � un dossier contenant des terrains
d�j� g�n�r�s. Dans ce cas, les terrains sont charg�s dynamiquement par
la classe Ground lorsque l'utilisateur le demande. Un terrain donn� est
consid�r� comme non g�n�r� s'il n'est ni dans le cache, ni dans le dossier
r�f�renc�. */
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
	
	/** Indique si le terrain � l'indice (x, y) existe d�j� ou au
	contraire doit �tre g�n�r� par le g�n�rateur de terrain.
	@returns true si le terrain existe, false s'il doit �tre g�n�r�.*/
	bool isTerrainGenerated(int x, int y, int lvl = 0) const;
	/** Donne le terrain d'indice (x, y). */
	const Terrain & getTerrain(int x, int y) const;

	std::string getTerrainDataId(int x, int y, int lvl) const;

	const Terrain & getTerrainAt(double x, double y, int lvl = 0) const;

	const std::vector<TerrainTile> getTerrainsFrom(const IPointOfView & pointOfView) const;
private:
	// Param�tres
	/** L'altitude maximum du monde. Le niveau de la mer est fix� � 0. */
	float _maxAltitude;
	/** L'altitude minimum du monde. Le niveau de la mer est fix� � 0. */
	float _minAltitude;
	/** La taille d'un terrain de niveau 0 utilis� pour paver la Map. Normalement,
	cette taille est la m�me que celle d'un pixel de la Map. */
	float _unitSize;

	// Donn�es
	int _cacheSize;
	mutable GroundCache * _cache;

	std::map<std::pair<int, int>, std::unique_ptr<Terrain>> & terrains() const;
	Terrain & terrain(int x, int y);

	friend class Environment2DGenerator; // TODO temporaire, trouver une solution plus propre
};


#endif //WORLD_TERRAINNODE_H
