#include "LODGridCoordinates.h"

#include <sstream>
#include <iomanip>

namespace world {

LODGridCoordinates LODGridCoordinates::NONE(0, 0, 0, -1);

LODGridCoordinates LODGridCoordinates::fromUID(const std::string &uid) {
    if (sizeof(int) * 4 > uid.length())
        throw std::runtime_error("bad usage : please provide a valid uid");

    const int *features = reinterpret_cast<const int *>(uid.c_str());
    return LODGridCoordinates(features[0], features[1], features[2],
                              features[3]);
}

LODGridCoordinates::LODGridCoordinates(int x, int y, int z, int lod)
        : _pos(x, y, z), _lod(lod) {}

LODGridCoordinates::LODGridCoordinates(const vec3i &pos, int lod)
        : _pos(pos), _lod(lod) {}

LODGridCoordinates::LODGridCoordinates(const LODGridCoordinates &other)
        : _pos(other._pos), _lod(other._lod) {}

LODGridCoordinates::~LODGridCoordinates() {}

const vec3i &LODGridCoordinates::getPosition3D() const { return _pos; }

std::string LODGridCoordinates::uid() const {
    int features[] = {_pos.x, _pos.y, _pos.z, _lod};
    return std::string((char *)features, sizeof(features));
}

bool LODGridCoordinates::operator<(const LODGridCoordinates &other) const {
    return _lod < other._lod ? true : _lod == other._lod && _pos < other._pos;
}

bool LODGridCoordinates::operator==(const LODGridCoordinates &other) const {
    return _lod == other._lod && _pos == other._pos;
}
} // namespace world
