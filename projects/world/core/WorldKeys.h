#ifndef WORLD_WORLDKEYS_H
#define WORLD_WORLDKEYS_H

#include "world/core/WorldConfig.h"

#include <iostream>
#include <string>
#include <vector>

namespace world {

typedef std::string NodeKey;

struct WORLDAPI_EXPORT NodeKeys {
    static NodeKey none() { return std::string(""); }

    static NodeKey fromUint(unsigned int id);
    static NodeKey fromInt(int id);

    static std::string toString(const NodeKey &key);
    static NodeKey fromString(const std::string &str);
};


typedef std::vector<std::string> ItemKey;

struct WORLDAPI_EXPORT ItemKeys {
    /** Returns a key refering to a root world node (ie which has no parent)*/
    static ItemKey root(const NodeKey &nodeKey) {
        ItemKey key{nodeKey};
        key.reserve(5);
        return key;
    }

    /** Returns a key refering to a world node which has a parent. */
    static ItemKey child(const ItemKey &parentKey, const NodeKey &nodeKey) {
        auto childKey = parentKey;
        childKey.push_back(nodeKey);
        return childKey;
    }

    static ItemKey defaultKey() {
        return {};
    }

    static ItemKey fromString(const std::string &str) {
        ItemKey result;
        size_t sep;
        size_t start = 0;

        while (sep != std::string::npos) {
            sep = str.find_first_of('/');
            std::string keystr;

            if (sep == std::string::npos) {
                keystr = str.substr(sep);
            }
            else {
                keystr = str.substr(start, sep - start);
                start = sep + 1;
            }

            try {
                result.push_back(NodeKeys::fromString(keystr));
            }
            catch (std::invalid_argument &e) {
                throw e;
            }
        }
        return result;
    }

    /** Gets an unique string representation for this key. The
     * string is printable and usable in a file system. */
    static std::string toString(const ItemKey &key) {
        std::string result;
        for (size_t i = 0; i < key.size(); ++i) {
            result += (i == 0 ? "" : "/") + NodeKeys::toString(key[i]);
        }
        return result;
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
