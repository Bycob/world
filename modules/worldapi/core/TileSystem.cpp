#include "TileSystem.h"

namespace world {

TileSystem::TileSystem(int maxLod, const vec3i &bufferRes,
                       const vec3d &baseSize)
        : _maxLod(maxLod), _bufferRes(bufferRes), _baseSize(baseSize) {}

int TileSystem::computeLod(double resolution, double baseSize,
                           int bufferRes) const {
    if (bufferRes == 0) {
        return _maxLod;
    }

    // resolution = _bufferRes * powi(_factor, lod) / _baseSize;
    return clamp(static_cast<int>(ceil(log(baseSize * resolution / bufferRes) /
                                       log(_factor))),
                 0, _maxLod);
}

int TileSystem::getLod(double resolution) const {
    int lodX = computeLod(resolution, _baseSize.x, _bufferRes.x);
    int lodY = computeLod(resolution, _baseSize.y, _bufferRes.y);
    int lodZ = computeLod(resolution, _baseSize.z, _bufferRes.z);

    return min(lodX, min(lodY, lodZ));
}

vec3d TileSystem::getTileFloatingCoordinates(const vec3d &globalCoordinates,
                                             int lod) const {
    vec3d tileSize = getTileSize(lod);
    const double eps = std::numeric_limits<double>::epsilon();
    return {tileSize.x > eps ? globalCoordinates.x / tileSize.x : 0,
            tileSize.y > eps ? globalCoordinates.y / tileSize.y : 0,
            tileSize.z > eps ? globalCoordinates.z / tileSize.z : 0};
}

TileCoordinates TileSystem::getTileCoordinates(const vec3d &globalCoordinates,
                                               int lod) const {
    vec3d tilePos = getTileFloatingCoordinates(globalCoordinates, lod);
    vec3i posi{static_cast<int>(floor(tilePos.x)),
               static_cast<int>(floor(tilePos.y)),
               static_cast<int>(floor(tilePos.z))};
    return TileCoordinates{posi, lod};
}

vec3d TileSystem::getLocalCoordinates(const vec3d &globalCoordinates,
                                      int lod) const {
    vec3d tilePos = getTileFloatingCoordinates(globalCoordinates, lod);
    return {tilePos.x - floor(tilePos.x), tilePos.y - floor(tilePos.y),
            tilePos.z - floor(tilePos.z)};
}

vec3d TileSystem::getTileOffset(const TileCoordinates &tileCoordinates) const {
    vec3d tileSize = getTileSize(tileCoordinates._lod);
    return tileCoordinates._pos * tileSize;
}

vec3d TileSystem::getTileSize(int lod) const {
    return _baseSize / powi(_factor, lod);
}

vec3d TileSystem::getGlobalCoordinates(const TileCoordinates &tileCoordinates,
                                       const vec3d &localCoordinates) const {
    vec3d tileSize = getTileSize(tileCoordinates._lod);
    return (tileCoordinates._pos + localCoordinates) * tileSize;
}

} // namespace world
