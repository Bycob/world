#include "LODGridCoordinates.h"

#include <sstream>
#include <iomanip>

namespace world {

LODGridCoordinates LODGridCoordinates::NONE(0, 0, 0, -1);

LODGridCoordinates LODGridCoordinates::getLastOfKey(const NodeKey &key) {
    const int keySize = sizeof(int) * 4;

    if (key.length() % keySize != 0)
        throw std::runtime_error("bad usage : please provide a valid key");

    if (key.empty())
        return NONE;

    const int *features = reinterpret_cast<const int *>(key.c_str() + key.length() - keySize);
    return LODGridCoordinates(features[0], features[1], features[2],
                              features[3]);
}

NodeKey LODGridCoordinates::getParent(const NodeKey &key) {
    const int keySize = sizeof(int) * 4;

    if (key.length() % keySize != 0)
        throw std::runtime_error("bad usage : please provide a valid key");

    if (key.empty())
        throw std::runtime_error("this key has no parent");

    return key.substr(0, key.length() - keySize);
}

LODGridCoordinates::LODGridCoordinates(int x, int y, int z, int lod)
        : _pos(x, y, z), _lod(lod) {}

LODGridCoordinates::LODGridCoordinates(const vec3i &pos, int lod)
        : _pos(pos), _lod(lod) {}

const vec3i &LODGridCoordinates::getPosition3D() const { return _pos; }

std::string LODGridCoordinates::toKey(const NodeKey &parent) const {
    int features[] = {_pos.x, _pos.y, _pos.z, _lod};
    return parent + std::string(reinterpret_cast<char *>(features), sizeof(features));
}

bool LODGridCoordinates::operator<(const LODGridCoordinates &other) const {
    return _lod < other._lod ? true : _lod == other._lod && _pos < other._pos;
}

bool LODGridCoordinates::operator==(const LODGridCoordinates &other) const {
    return _lod == other._lod && _pos == other._pos;
}
} // namespace world
