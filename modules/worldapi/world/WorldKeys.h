#ifndef WORLD_WORLDKEYS_H
#define WORLD_WORLDKEYS_H

#include <worldapi/worldapidef.h>

#include <string>

namespace world {

    typedef std::string ChunkKey;
    typedef int ObjectKey;
    typedef int AssetKey;

    struct ChunkKeys {
        static std::string none() {
            return std::string("");
        }
    };

    struct ObjectKeys {
        constexpr static int defaultKey() {
            return 0;
        }
    };
}

#endif //WORLD_WORLDKEYS_H
