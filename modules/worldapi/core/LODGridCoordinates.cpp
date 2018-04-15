#include "LODGridCoordinates.h"

#include <sstream>
#include <iomanip>

namespace world {

LODGridCoordinates LODGridCoordinates::NONE(0, 0, 0, -1);

LODGridCoordinates::LODGridCoordinates(int x, int y, int z, int lod)
        : _pos(x, y, z), _lod(lod) {}

LODGridCoordinates::LODGridCoordinates(const vec3i &pos, int lod)
        : _pos(pos), _lod(lod) {}

LODGridCoordinates::LODGridCoordinates(const LODGridCoordinates &other)
        : _pos(other._pos), _lod(other._lod) {}

LODGridCoordinates::~LODGridCoordinates() {}

const vec3i &LODGridCoordinates::getPosition3D() const { return _pos; }

std::string LODGridCoordinates::uid() const {
    int features[] = {_pos.x, _pos.y, _pos.z, _lod, 0};
    return std::string((char *)features,
                       sizeof(features) - sizeof(*features) + 1);
}

bool LODGridCoordinates::operator<(const LODGridCoordinates &other) const {
    return _lod < other._lod ? true : _lod == other._lod && _pos < other._pos;
}

bool LODGridCoordinates::operator==(const LODGridCoordinates &other) const {
    return _lod == other._lod && _pos == other._pos;
}
} // namespace world
