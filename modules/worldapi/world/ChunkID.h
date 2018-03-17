#ifndef WORLD_CHUNKID_H
#define WORLD_CHUNKID_H

#include <worldapi/worldapidef.h>

namespace world {
    class WORLDAPI_EXPORT ChunkID {
    public:
        static ChunkID NONE;

        ChunkID(int x, int y, int z, int lod = 0);

        ChunkID(const vec3i &pos, int lod = 0);

        ChunkID(const ChunkID &other);

        ~ChunkID();

        const vec3i &getPosition3D() const;

        int getLOD() const { return _lod; }

        bool operator<(const ChunkID &other) const;

        bool operator==(const ChunkID &other) const;

    private:
        vec3i _pos;
        int _lod;
    };
}


#endif //WORLD_CHUNKID_H
