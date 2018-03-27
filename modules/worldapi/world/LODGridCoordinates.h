#ifndef WORLD_CHUNKID_H
#define WORLD_CHUNKID_H

#include <worldapi/worldapidef.h>

#include "../maths/Vector.h"

namespace world {
    class WORLDAPI_EXPORT LODGridCoordinates {
    public:
        static LODGridCoordinates NONE;

        LODGridCoordinates(int x, int y, int z, int lod = 0);

        LODGridCoordinates(const vec3i &pos, int lod = 0);

        LODGridCoordinates(const LODGridCoordinates &other);

        ~LODGridCoordinates();

        const vec3i &getPosition3D() const;

        int getLOD() const { return _lod; }

        bool operator<(const LODGridCoordinates &other) const;

        bool operator==(const LODGridCoordinates &other) const;

    private:
        vec3i _pos;
        int _lod;
    };
}


#endif //WORLD_CHUNKID_H
