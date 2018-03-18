#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include <worldapi/worldapidef.h>

#include <map>
#include <utility>
#include <functional>

#include "Terrain.h"
#include "../world/ChunkSystem.h"
#include "../world/MapGenerator.h"

namespace world {

	class FlatWorld;

	class FlatWorldCollector;

	class PrivateGround;

	/** Cette classe gère le sol du monde. Le sol est composé de plusieurs
	terrains carrés accolés les uns aux autres. Des détails sont ensuite
	ajoutés pour les différents niveaux de détail.
	Cette classe peut faire référence à un dossier contenant des terrains
	déjà générés. Dans ce cas, les terrains sont chargés dynamiquement par
	la classe Ground lorsque l'utilisateur le demande. Un terrain donné est
	considéré comme non généré s'il n'est ni dans le cache, ni dans le dossier
	référencé. */
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
		double observeAltitudeAt(WorldZone zone, double x, double y);

		void collectZone(FlatWorldCollector &collector, FlatWorld &world, WorldZone &zone);

	private:
		PrivateGround *_internal;

		// Paramètres
		/** L'altitude maximum du monde. Le niveau de la mer est fixé à 0. */
		double _maxAltitude;
		/** L'altitude minimum du monde. Le niveau de la mer est fixé à 0. */
		double _minAltitude;
		/** La taille d'un terrain de niveau 0 utilisé pour paver la Map. Normalement,
        cette taille est la même que celle d'un pixel de la Map. */
		double _unitSize;
		/** Le facteur de subdivision pour les différents niveaux de détails.
         * La taille d'un terrain en fonction de son niveau de détail est
         * calculée ainsi : _unitSize * _factor ^ lod */
		int _factor = 2;
		int _terrainRes = 129;
		int _maxLOD = 4;

		// Generator
		std::unique_ptr<ReliefMapGenerator> _mapGenerator;
		std::unique_ptr<TerrainGenerator> _terrainGenerator;


		double observeAltitudeAt(double x, double y, int lvl = 0);

		/** Replace a parent terrain by its children in the collector */
		void replaceTerrain(int x, int y, int lvl, FlatWorldCollector &collector);

		// ACCESS
		Terrain &terrain(int x, int y, int lvl = 0);

		Terrain &rawTerrain(int x, int y, int lvl = 0);

		/** Indique si le terrain à l'indice (x, y) existe déjà ou au
        contraire doit être généré par le générateur de terrain.
        @returns true si le terrain existe, false s'il doit être généré.*/
		bool terrainExists(int x, int y, int lvl = 0) const;


		// DATA
		/** Donne un identifiant unique pour la section de terrain à
         * l'emplacement donné. */
		uint64_t getTerrainDataId(int x, int y, int lvl) const;

		double getTerrainSize(int level) const;

		int getLevelForChunk(const WorldZone &zone) const;

		vec3i getParentId(const vec3i &childId) const;


		// GENERATION
		void generateZone(FlatWorld &world, WorldZone &zone);

		/** Generate the terrain with given coordinates. Assume that:
         * - the terrain wasn't generated yet,
         * - the terrains with higher level at the same place are already
         * generated.*/
		void generateTerrain(int x, int y, int lvl);

		void applyPreviousLayer(int x, int y, int lvl, bool unapply = false);

		void applyParent(int tX, int tY, int lvl, bool unapply = false);

		void applyMap(int x, int y, int lvl, bool unapply = false);

		std::pair<std::unique_ptr<Terrain>, std::unique_ptr<Terrain>> & getOrCreateMap(int x, int y);

		Terrain& getOrCreateOffsetMap(int x, int y);

		Terrain& getOrCreateDiffMap(int x, int y);

		friend class GroundContext;
	};
}

#endif //WORLD_TERRAINNODE_H
