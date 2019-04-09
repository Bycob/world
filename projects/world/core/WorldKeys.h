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


class WORLDAPI_EXPORT ItemKey {
public:
    std::vector<NodeKey> _components;


    ItemKey() = default;

    explicit ItemKey(const std::vector<NodeKey> &components)
        : _components(components) {}

    ItemKey(const NodeKey &key) {
        _components.push_back(key);
    }
    ItemKey(const ItemKey &parent, const NodeKey &key)
        : _components(parent._components){

        _components.push_back(key);
    }

    ItemKey(const ItemKey &key1, const ItemKey &key2)
        : _components(key1._components) {

        _components.insert(_components.end(), key2._components.begin(), key2._components.end());
    }

    std::string str() const {
        std::string result;
        for (size_t i = 0; i < _components.size(); ++i) {
            result += (i == 0 ? "" : "/") + NodeKeys::toString(_components[i]);
        }
        return result;
    }

    ItemKey parent() const {
        if (_components.empty()) {
            throw std::runtime_error("no parent");
        }

        ItemKey result = *this;
        result._components.pop_back();
        return result;
    }

    NodeKey last() const {
        return _components.back();
    }

    bool operator==(const ItemKey &other) const {
        return _components == other._components;
    }

    bool operator !=(const ItemKey &other) const {
        return _components != other._components;
    }

    bool operator<(const ItemKey &other) const {
        return _components < other._components;
    }
};

struct WORLDAPI_EXPORT ItemKeys {
    /** Returns a key refering to a root world node (ie which has no parent)*/
    static ItemKey root(const NodeKey &nodeKey) {
        return {nodeKey};
    }

    /** Returns a key refering to a world node which has a parent. */
    static ItemKey child(const ItemKey &parentKey, const NodeKey &nodeKey) {
        return {parentKey, nodeKey};
    }

    /** Return a key beggining with "prefix" and ending with "suffix". */
    static ItemKey concat(const ItemKey &prefix, const ItemKey &suffix) {
        return {prefix, suffix};
    }

    static ItemKey getParent(const ItemKey &key) {
        return key.parent();
    }

    static NodeKey getLastNode(const ItemKey &key) {
        return key.last();
    }

    static ItemKey defaultKey() { return {}; }

    static ItemKey fromString(const std::string &str) {
        ItemKey result;
        size_t sep;
        size_t start = 0;

        do {
            sep = str.find_first_of('/', start);
            std::string keystr;

            if (sep == std::string::npos) {
                keystr = str.substr(start);
            } else {
                keystr = str.substr(start, sep - start);
                start = sep + 1;
            }

            try {
                result._components.push_back(NodeKeys::fromString(keystr));
            } catch (std::invalid_argument &e) {
                throw e;
            }
        } while (sep != std::string::npos);

        return result;
    }

    /** Gets an unique string representation for this key. The
     * string is printable and usable in a file system. */
    static std::string toString(const ItemKey &key) {
        return key.str();
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
