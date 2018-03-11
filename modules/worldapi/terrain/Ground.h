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
#include "../world/ChunkSystem.h"
#include "../world/MapGenerator.h"

class FlatWorld;
class GroundCache;

/** Cette classe g�re le sol du monde. Le sol est compos� de plusieurs
terrains carr�s accol�s les uns aux autres. Des d�tails sont ensuite
ajout�s pour les diff�rents niveaux de d�tail.
Cette classe peut faire r�f�rence � un dossier contenant des terrains
d�j� g�n�r�s. Dans ce cas, les terrains sont charg�s dynamiquement par
la classe Ground lorsque l'utilisateur le demande. Un terrain donn� est
consid�r� comme non g�n�r� s'il n'est ni dans le cache, ni dans le dossier
r�f�renc�. */
class WORLDAPI_EXPORT Ground {
public:
    Ground();
	virtual ~Ground();

	// PARAMETERS
	// TODO constraints
	void setMaxAltitude(double max) { _maxAltitude = max; }
	void setMinAltitude(double min) { _minAltitude = min; }
	void setUnitSize(double unitSize) { _unitSize = unitSize; }

	double getMaxAltitude() const { return _maxAltitude; }
	double getMinAltitude() const { return _minAltitude; }
	double getAltitudeRange() const { return _maxAltitude - _minAltitude; }
	double getUnitSize() const { return _unitSize; }

	// EXPLORATION
	double observeAltitudeAt(double x, double y, int lvl = 0);
private:
	// Param�tres
	/** L'altitude maximum du monde. Le niveau de la mer est fix� � 0. */
	double _maxAltitude;
	/** L'altitude minimum du monde. Le niveau de la mer est fix� � 0. */
	double _minAltitude;
	/** La taille d'un terrain de niveau 0 utilis� pour paver la Map. Normalement,
	cette taille est la m�me que celle d'un pixel de la Map. */
	double _unitSize;
	/** Le facteur de subdivision pour les diff�rents niveaux de d�tails.
	 * La taille d'un terrain en fonction de son niveau de d�tail est
	 * calcul�e ainsi : _unitSize * _factor ^ lod */
    int _factor = 4;

    // Generator
    std::unique_ptr<ReliefMapGenerator> _mapGenerator;
    std::unique_ptr<TerrainGenerator> _terrainGenerator;

	// Donn�es
	int _cacheSize;
	mutable GroundCache * _cache;

	// ACCESS
	std::map<maths::vec3i, std::unique_ptr<Terrain>> & terrains() const;
	Terrain & terrain(int x, int y, int lvl = 0);
	Terrain & terrainAt(double x, double y, int lvl = 0);

	/** Indique si le terrain � l'indice (x, y) existe d�j� ou au
	contraire doit �tre g�n�r� par le g�n�rateur de terrain.
	@returns true si le terrain existe, false s'il doit �tre g�n�r�.*/
	bool terrainExists(int x, int y, int lvl = 0) const;

	// DATA
	/** Donne un identifiant unique pour la section de terrain �
	 * l'emplacement donn�. */
	std::string getTerrainDataId(int x, int y, int lvl) const;
    double getTerrainSize(int level) const;

	// GENERATION
    void generateChunk(FlatWorld &world, ChunkNode &chunk);
	/** Generate the terrain with given coordinates. Assume that:
	 * - the terrain wasn't generated yet,
	 * - the terrains with higher level at the same place are already
	 * generated.*/
    void generateTerrain(int x, int y, int lvl);
    void applyMap(int x, int y, int lvl, bool unapply);
};


#endif //WORLD_TERRAINNODE_H
