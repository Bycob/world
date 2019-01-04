#ifndef WORLD_TILE_SYSTEM_H
#define WORLD_TILE_SYSTEM_H

#include "world/core/WorldConfig.h"

#include "world/math/Vector.h"
#include "IResolutionModel.h"
#include "WorldZone.h"

namespace world {

class TileSystem;
class TileSystemIterator;

/** A unique identifier for a tile in one tile system.
 * This identifier is compound of an integer position
 * and a level of detail.
 */
struct WORLDAPI_EXPORT TileCoordinates {
    TileCoordinates() {}
    TileCoordinates(int x, int y, int z, int lod) : _pos{x, y, z}, _lod(lod) {}
    TileCoordinates(const vec3i &pos, int lod) : _pos(pos), _lod(lod) {}

    vec3i _pos;
    int _lod = 0;
};

inline bool operator<(const TileCoordinates &coord1,
                      const TileCoordinates &coord2) {
    return coord1._lod < coord2._lod
               ? true
               : (coord1._lod == coord2._lod && coord1._pos < coord2._pos);
}

/** This class performs conversion between the world coordinates
 * system and a defined tile based data structure with different
 * levels of detail.
 *
 * A TileSystem instance perform interfacing between the global
 * frame, the tile ids and the local position inside of a tile.
 * A tile id is a struct compound of an integer position and a
 * level of detail. The local position is expressed in terms of
 * "tile percentage", so each of its components is between 0 and 1.
 *
 * Each field of the TileSystem can be modified during usage, but
 * this may break iterators created from this TileSystem. */
class WORLDAPI_EXPORT TileSystem {
public:
    int _factor = 2;

    int _maxLod;

    vec3i _bufferRes;

    vec3d _baseSize;

    /** Build a TileSystem with the given parameters.
     *
     * \param maxLod Maximum level of detail held by this TileSystem. Minimum is
     * 0.
     *
     * \param bufferRes The resolution of each buffer corresponding to a
     * tile, in x, y and z axis.
     *
     * \param baseSize Size of a tile at LOD 0 */
    TileSystem(int maxLod, const vec3i &bufferRes, const vec3d &baseSize);

    // GLOBAL -> TILE

    int getLod(double resolution) const;

    TileCoordinates getTileCoordinates(const vec3d &globalCoordinates,
                                       int lod) const;

    vec3d getLocalCoordinates(const vec3d &globalCoordinates, int lod) const;

    // TILE -> GLOBAL

    vec3d getTileOffset(const TileCoordinates &tileCoordinates) const;

    vec3d getTileSize(int lod) const;

    vec3d getGlobalCoordinates(const TileCoordinates &tileCoordinates,
                               const vec3d &localCoordinates) const;

    // ---

    TileCoordinates getParentTileCoordinates(
        const TileCoordinates &childCoordinates) const;

    /** Iterates over all the visible tiles in the given resolution model,
     * inside of the given zone. Iterated tiles are sorted as if operator< was
     * used. */
    TileSystemIterator iterate(const IResolutionModel &resolutionModel,
                               const WorldZone &zone) const;

    /** Iterates over all the visible tiles in the given resolution model,
     * inside of the zone delimited by the given bounds. */
    TileSystemIterator iterate(const IResolutionModel &resolutionModel,
                               const BoundingBox &bounds) const;

private:
    int computeLod(double resolution, double baseSize, int bufferRes) const;

    vec3d getTileFloatingCoordinates(const vec3d &globalCoordinates,
                                     int lod) const;
};

/** Iterator over tiles in a particular region. A TileSystemIterator uses a
 * TileSystem to know which tiles should be iterated over. To use the iterator,
 * make a for loop, like this:
 *
 * \code{.cpp} auto it = TileSystemIterator(...);
 * for (; !it.endReached(); ++it) {
 *    // Do stuff
 * }
 * \endcode
 */
class WORLDAPI_EXPORT TileSystemIterator {
public:
    TileSystemIterator(const TileSystem &tileSystem,
                       const IResolutionModel &resolutionModel,
                       const WorldZone &bounds);

    TileSystemIterator(const TileSystem &tileSystem,
                       const IResolutionModel &resolutionModel,
                       const BoundingBox &bounds);

    void operator++();

    TileCoordinates operator*();

    bool endReached() const;

private:
    const TileSystem &_tileSystem;
    const IResolutionModel &_resolutionModel;

    BoundingBox _bounds;

    TileCoordinates _current;
    TileCoordinates _min;
    TileCoordinates _max;
    bool _endReached = false;

    // Increments current tile coordinates without taking the resolution model
    // into account.
    void step();

    // Once a lod was fully iterated over, this method can be used to start
    // iterating the next one.
    void startLod(int lod);

    // Indicates if the tile at given coordinates needs to be displayed. Wether
    // a tile needs or not to be displayed is determined by the tile system.
    bool isTileRequired(TileCoordinates coordinates);
};

} // namespace world

#endif // WORLD_TILE_SYSTEM_H
