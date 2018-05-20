#ifndef WORLD_TILE_SYSTEM_H
#define WORLD_TILE_SYSTEM_H

#include "core/WorldConfig.h"

#include "math/Vector.h"

namespace world {

/** A unique identifier for a tile in one tile system.
 * This identifier is compound of an integer position
 * and a level of detail.
 */
struct WORLDAPI_EXPORT TileCoordinates {
    vec3i _pos;
    int _lod = 0;
};

/** This class performs conversion between the world coordinates
 * system and a defined tile based data structure with different
 * levels of detail.
 *
 * A TileSystem instance perform interfacing between the global
 * frame, the tile ids and the local position inside of a tile.
 * A tile id is a struct compound of an integer position and a
 * level of detail. The local position is expressed in terms of
 * "tile percentage", so each of its components is between 0 and 1. */
class WORLDAPI_EXPORT TileSystem {
public:
    int _factor = 2;

    int _maxLod;

    vec3i _bufferRes;

    vec3d _baseSize;


    TileSystem(int maxLod, const vec3i &bufferRes, const vec3d &baseSize);

    int getLod(double resolution) const;

    TileCoordinates getTileCoordinates(const vec3d &globalCoordinates,
                                       int lod) const;

    vec3d getLocalCoordinates(const vec3d &globalCoordinates, int lod) const;


    vec3d getTileOffset(const TileCoordinates &tileCoordinates) const;

    vec3d getTileSize(int lod) const;

    vec3d getGlobalCoordinates(const TileCoordinates &tileCoordinates,
                               const vec3d &localCoordinates) const;

private:
    int computeLod(double resolution, double baseSize, int bufferRes) const;

    vec3d getTileFloatingCoordinates(const vec3d &globalCoordinates,
                                     int lod) const;
};

} // namespace world

#endif // WORLD_TILE_SYSTEM_H
