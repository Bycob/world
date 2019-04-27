#ifndef WORLD_CHUNKID_H
#define WORLD_CHUNKID_H

#include "world/core/WorldConfig.h"

#include "WorldKeys.h"
#include "world/math/Vector.h"

namespace world {
class WORLDAPI_EXPORT LODGridCoordinates {
public:
    static LODGridCoordinates NONE;


    LODGridCoordinates(int x, int y, int z, int lod = 0);

    LODGridCoordinates(const vec3i &pos, int lod = 0);

    const vec3i &getPosition3D() const;

    int getLOD() const { return _lod; }

    bool operator<(const LODGridCoordinates &other) const;

    bool operator==(const LODGridCoordinates &other) const;


    // Conversion to NodeKey

    static LODGridCoordinates getLastOfKey(const NodeKey &key);

    static NodeKey getParent(const NodeKey &key);

    NodeKey toKey(const NodeKey &parent = NodeKeys::none()) const;

private:
    vec3i _pos;
    int _lod;
};
} // namespace world


#endif // WORLD_CHUNKID_H
