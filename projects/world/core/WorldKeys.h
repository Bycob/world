#ifndef WORLD_WORLDKEYS_H
#define WORLD_WORLDKEYS_H

#include "world/core/WorldConfig.h"

#include <iostream>
#include <string>
#include <tuple>

namespace world {

typedef std::string ChunkKey;
typedef int ObjectKey;
typedef int AssetKey;

struct WORLDAPI_EXPORT ChunkKeys {
    static std::string none() { return std::string(""); }

    static std::string toString(const ChunkKey &key);
    static ChunkKey fromString(const std::string &str);
};

struct WORLDAPI_EXPORT ObjectKeys {
    constexpr static int defaultKey() { return 0; }

    static std::string toString(const ObjectKey &key) {
        return std::to_string(key);
    }
    static ObjectKey fromString(const std::string &str);
};

struct WORLDAPI_EXPORT AssetKeys {
    constexpr static int defaultKey() { return 0; }

    static std::string toString(const AssetKey &key) {
        return std::to_string(key);
    }
    static AssetKey fromString(const std::string &str);
};

typedef std::tuple<ChunkKey, ObjectKey, AssetKey> ItemKey;


struct WORLDAPI_EXPORT ItemKeys {
    /** Generates a key from the world's perspective : we need to
     * identify the chunk, the object and the part of the object.*/
    static ItemKey inWorld(const ChunkKey &chunkKey, const ObjectKey &objKey,
                           const AssetKey &assetKey) {
        return std::make_tuple(chunkKey, objKey, assetKey);
    }

    /** Generates a key from inside a chunk. We just need to
     * specify which object and which part of it we want a key for. */
    static ItemKey inChunk(const ObjectKey &objKey, const AssetKey &assetKey) {
        return inWorld(ChunkKeys::none(), objKey, assetKey);
    }

    /** Generates a key from inside an object. We just need to
     * specify which part of the object we want a key for. */
    static ItemKey inObject(const AssetKey &assetKey) {
        return inWorld(ChunkKeys::none(), ObjectKeys::defaultKey(), assetKey);
    }

    static ItemKey defaultKey() {
        return inWorld(ChunkKeys::none(), ObjectKeys::defaultKey(),
                       AssetKeys::defaultKey());
    }

    static ItemKey fromString(const std::string &str) {
        size_t sep1 = str.find_first_of('/');
        size_t sep2 = str.find_first_of('/', sep1 + 1);

        if (sep1 == std::string::npos && sep2 == std::string::npos) {
            throw std::invalid_argument("not a ItemKey");
        }

        try {
            return std::make_tuple(
                ChunkKeys::fromString(str.substr(0, sep1)),
                ObjectKeys::fromString(str.substr(sep1 + 1, sep2 - sep1 - 1)),
                AssetKeys::fromString(str.substr(sep2 + 1)));
        } catch (std::invalid_argument &e) {
            throw e;
        }
    }

    /** Gets an unique string representation for this key. The
     * string is printable and usable in a file system. */
    static std::string toString(const ItemKey &key) {
        return world::ChunkKeys::toString(std::get<0>(key)) + "/" +
               world::ObjectKeys::toString(std::get<1>(key)) + "/" +
               world::AssetKeys::toString(std::get<2>(key));
    }
};

inline ItemKey key(const std::string &str) {
    try {
        return ItemKeys::fromString(str);
    } catch (std::invalid_argument &) {
        return ItemKeys::defaultKey();
    }
}

} // namespace world

#endif // WORLD_WORLDKEYS_H
