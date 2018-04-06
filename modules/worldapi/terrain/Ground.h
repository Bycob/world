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

	/** Cette classe gère le sol du monde. Le sol est composé de plusieurs
	terrains carrés accolés les uns aux autres. Des détails sont ensuite
	ajoutés pour les différents niveaux de détail.
	Cette classe peut faire référence à un dossier contenant des terrains
	déjà générés. Dans ce cas, les terrains sont chargés dynamiquement par
	la classe Ground lorsque l'utilisateur le demande. Un terrain donné est
	considéré comme non généré s'il n'est ni dans le cache, ni dans le dossier
	référencé. */
	class WORLDAPI_EXPORT Ground : public IGround {
	public:
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

		void collectZone(FlatWorldCollector &collector, FlatWorld &world, const WorldZone &zone) override;

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
		int _terrainRes = 65;
		/** Texture resolution, relatively to the terrain resolution */
		int _textureRes = 4;
		int _maxLOD = 3;


		double observeAltitudeAt(double x, double y, int lvl);

		/** Replace a parent terrain by its children in the collector */
		void replaceTerrain(int x, int y, int lvl, FlatWorldCollector &collector);

		void addTerrain(int x, int y, int lvl, ICollector &collector);


		// ACCESS
		Terrain &terrain(int x, int y, int lvl);

		optional<Mesh &> mesh(int x, int y, int lvl);

		optional<Terrain &> cachedTerrain(int x, int y, int lvl, int genID);

		/** Indique si le terrain à l'indice (x, y) existe déjà ou au
        contraire doit être généré par le générateur de terrain.
        @returns true si le terrain existe, false s'il doit être généré.*/
		bool terrainExists(int x, int y, int lvl) const;


		// DATA
		/** Donne un identifiant unique pour la section de terrain à
         * l'emplacement donné. */
		std::string getTerrainDataId(int x, int y, int lvl) const;

		double getTerrainSize(int level) const;

		double getTerrainResolution(int level) const;

		int getLevelForChunk(const WorldZone &zone) const;

		vec3i getParentId(const vec3i &childId) const;


		// GENERATION
		void generateZone(FlatWorld &world, const WorldZone &zone);

		/** Generate the terrain with given coordinates. Assume that:
         * - the terrain wasn't generated yet,
         * - the terrains with higher level at the same place are already
         * generated.*/
		void generateTerrain(int x, int y, int lvl);

		friend class GroundContext;
	};
}

#endif //WORLD_TERRAINNODE_H
