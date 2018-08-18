#ifndef WORLD_WORLDKEYS_H
#define WORLD_WORLDKEYS_H

#include "core/WorldConfig.h"

#include <string>

namespace world {

typedef std::string ChunkKey;
typedef int ObjectKey;
typedef int AssetKey;

struct WORLDAPI_EXPORT ChunkKeys {
    static std::string none() { return std::string(""); }

    static std::string toString(const ChunkKey &key);
};

struct ObjectKeys {
    constexpr static int defaultKey() { return 0; }

    static std::string toString(const ObjectKey &key) {
        return std::to_string(key);
    }
};

struct AssetKeys {
	constexpr static int defaultKey() { return 0; }

    static std::string toString(const AssetKey &key) {
        return std::to_string(key);
    }
};
} // namespace world

#endif // WORLD_WORLDKEYS_H
