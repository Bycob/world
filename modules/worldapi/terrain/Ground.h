#ifndef WORLD_TERRAINNODE_H
#define WORLD_TERRAINNODE_H

#include "core/WorldConfig.h"

#include <map>
#include <utility>
#include <functional>

#include "core/WorldZone.h"
#include "flat/IGround.h"
#include "Terrain.h"
#include "ITerrainWorker.h"

namespace world {

	class PrivateGround;

	/** This class manages an infinite ground with as much details
	 * as we want. The access method are quite similar to the ones
	 * used on the WorldObjects : you can get parts of the ground by
	 * specifying which part of the world you're wanting to get content
	 * from. */
	class WORLDAPI_EXPORT Ground : public IGround {
	public:
        struct TerrainKey;

        struct Tile;

		Ground(double unitSize = 6000, double minAltitude = -2000, double maxAltitude = 4000);

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
		double observeAltitudeAt(WorldZone zone, double x, double y) override;

		void collectZone(const WorldZone &zone, ICollector &collector);

		void collectZone(const WorldZone &zone, FlatWorld &world, FlatWorldCollector &collector) override;

	private:
		PrivateGround *_internal;

		// Paramètres
		/** L'altitude minimum du monde. Le niveau de la mer est fixé à 0. */
		double _minAltitude;
		/** L'altitude maximum du monde. Le niveau de la mer est fixé à 0. */
		double _maxAltitude;
		/** La taille d'un terrain de niveau 0 utilisé pour paver la Map. Normalement,
        cette taille est la même que celle d'un pixel de la Map. */
		double _unitSize;
		/** Le facteur de subdivision pour les différents niveaux de détails.
         * La taille d'un terrain en fonction de son niveau de détail est
         * calculée ainsi : _unitSize * _factor ^ lod */

		int _factor = 4;
		int _terrainRes = 33;
		/** Texture resolution, relatively to the terrain resolution */
		int _textureRes = 8;
		int _maxLOD = 3;

		int _maxCacheSize = 750;

		double observeAltitudeAt(double x, double y, int lvl);

		/** Replace a parent terrain by its children in the collector */
		void replaceTerrain(const TerrainKey &key, ICollector &collector);

		void addTerrain(const TerrainKey &key, ICollector &collector);

		/** Updates the cache, free old used memory if needed (by saving
		 * the terrains to files or discard them) */
		void updateCache();


		// ACCESS
        Ground::Tile &provide(const TerrainKey &key);

        void registerAccess(const TerrainKey &key, Tile &tile);

		Terrain &provideTerrain(const TerrainKey &key);

		optional<Mesh &> provideMesh(const TerrainKey &key);

		optional<Terrain &> getCachedTerrain(const TerrainKey &key, int genID);


        // DATA
		/** Gets a unique string id for the given tile in the Ground. */
		std::string getTerrainDataId(const TerrainKey &key) const;

		double getTerrainSize(int level) const;

		double getTerrainResolution(int level) const;

		int getLevelForChunk(const WorldZone &zone) const;

		TerrainKey getParentKey(const TerrainKey &childId) const;


		// GENERATION
		/** Generate the terrain with given coordinates. Assume that:
         * - the terrain wasn't generated yet,
         * - the terrains with higher level at the same place are already
         * generated.*/
		void generateTerrain(const TerrainKey &key);

		friend class PrivateGround;
		friend class GroundContext;
	};
}

#endif //WORLD_TERRAINNODE_H
