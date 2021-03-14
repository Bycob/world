#ifndef WORLD_WORLDKEYS_H
#define WORLD_WORLDKEYS_H

#include "world/core/WorldConfig.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace world {

// Key refactor:
// use a string literal to transform a string to a key
// conversion from NodeKey to string is implicit
// string literal (_s) to convert itemkey to string ?

class ItemKey;

const char KEY_SEPARATOR = '/';
constexpr char KEY_SEPARATOR_STR[] = {KEY_SEPARATOR, '\0'};

/** A key for a node in a world. Must only contain alphanumeric characters,
 * '_', '-' or '.'. */
class WORLDAPI_EXPORT NodeKey {
public:
    std::string _name;

    NodeKey(std::string name) : _name(std::move(name)) {}

    NodeKey(const char *name) : _name(name) {}

    explicit NodeKey(int id);

    /** Check if the key contains only valid characters (alphanumeric
     * characters, '_', '-' or '.'). */
    bool valid() const;

    bool operator==(const NodeKey &other) const;

    bool operator!=(const NodeKey &other) const;

    bool operator<(const NodeKey &other) const;

    bool operator>(const NodeKey &other) const;

    operator std::string() const { return _name; }

    std::string str() const { return _name; }
};

inline NodeKey operator"" _k(const char *keyStr, size_t) {
    return NodeKey(keyStr);
}

// TODO change to ItemKey & ItemPath

/**
 * ItemKeys are used to reference items in the world. Items can be
 * WorldNodes or assets within WorldNodes, and ItemKeys are used
 * in many places including world internal structures, collector and
 * cache on hard drive.
 *
 * An ItemKey is compound of multiple NodeKeys that make up a path
 * to the resource they reference. For example, if a resource is produced
 * by a WorldNode, its key will be the concatenation of the parent node
 * and the resource specific key.
 *
 * Path are corresponding exactly to file path on cache, so the keys
 * can only hold valid characters for file names, and the different
 * keys that form a path are separated by '/'.
 *
 * Here is a list of some of the possible operations on keys:
 * - concatenation of two keys: key = { key1, key2 } with keyx as a path / key
 * - get last part of a key: nodeKey = key1.last()
 * - comparison between keys (for mapping): key1 < key2
 * - conversion to path string: key.str()
 */
class WORLDAPI_EXPORT ItemKey {
public:
    std::vector<NodeKey> _components;


    ItemKey() = default;

    ItemKey(const NodeKey &key) { _components.push_back(key); }

    ItemKey(const ItemKey &key1, const ItemKey &key2)
            : _components(key1._components) {

        _components.insert(_components.end(), key2._components.begin(),
                           key2._components.end());
    }

    ItemKey(std::vector<NodeKey> components)
            : _components(std::move(components)) {}


    std::string str() const;

    ItemKey parent() const;

    NodeKey last() const { return _components.back(); }

    bool operator==(const ItemKey &other) const {
        return _components == other._components;
    }

    bool operator!=(const ItemKey &other) const {
        return _components != other._components;
    }

    bool operator<(const ItemKey &other) const {
        return _components < other._components;
    }

    bool operator>(const ItemKey &other) const {
        return _components > other._components;
    }
};


// LEGACY (for backward compatibility)
struct WORLDAPI_EXPORT NodeKeys {
    static NodeKey none() { return std::string(""); }

    static NodeKey fromUint(unsigned int id);
    static NodeKey fromInt(int id);

    static std::string toString(const NodeKey &key);
    static NodeKey fromString(const std::string &str);
};

struct WORLDAPI_EXPORT ItemKeys {
    /** Returns a key refering to a root world node (ie which has no parent)*/
    static ItemKey root(const NodeKey &nodeKey) { return {nodeKey}; }

    /** Returns a key refering to a world node which has a parent. */
    static ItemKey child(const ItemKey &parentKey, const NodeKey &nodeKey) {
        return {parentKey, nodeKey};
    }

    /** Return a key beggining with "prefix" and ending with "suffix". */
    static ItemKey concat(const ItemKey &prefix, const ItemKey &suffix) {
        return {prefix, suffix};
    }

    static ItemKey getParent(const ItemKey &key) { return key.parent(); }

    static NodeKey getLastNode(const ItemKey &key) { return key.last(); }

    static ItemKey defaultKey() { return {}; }

    static ItemKey fromString(const std::string &str);

    /** Gets an unique string representation for this key. The
     * string is printable and usable in a file system. */
    static std::string toString(const ItemKey &key) { return key.str(); }
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
