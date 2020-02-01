#include "TileSystem.h"

namespace world {

TileSystem::TileSystem(int maxLod, const vec3i &bufferRes,
                       const vec3d &baseSize)
        : _maxLod(maxLod), _bufferRes(bufferRes), _baseSize(baseSize) {}

int TileSystem::computeLod(double resolution, double baseSize, int bufferRes,
                           int maxLod) const {

    for (int lod = 0; bufferRes != 0 && lod < maxLod; ++lod) {
        if (resolution <= bufferRes * powi(_factor, lod) / baseSize)
            return lod;
    }

    return maxLod;
}

int TileSystem::getLod(double resolution) const {
    int lodX = computeLod(resolution, _baseSize.x, _bufferRes.x, _maxLod);
    int lodY = computeLod(resolution, _baseSize.y, _bufferRes.y, lodX);
    int lodZ = computeLod(resolution, _baseSize.z, _bufferRes.z, lodY);

    return lodZ;
}

double TileSystem::getMinResolution(int lod) const {
    if (lod == 0) {
        return 0;
    } else {
        // TODO use the max of all factors
        double ref = _bufferRes.x / _baseSize.x;
        return ref * powi(_factor, lod);
    }
}

double TileSystem::getMaxResolution(int lod) const {
    if (lod == _maxLod) {
        return 1e100;
    } else {
        return getMinResolution(lod + 1);
    }
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

TileCoordinates TileSystem::getParentTileCoordinates(
    const TileCoordinates &childCoordinates) const {
    double f = _factor;
    vec3d parentCoordinates = childCoordinates._pos / f;
    return TileCoordinates(parentCoordinates.floor(),
                           childCoordinates._lod - 1);
}

TileSystemIterator TileSystem::iterate(const IResolutionModel &resolutionModel,
                                       const BoundingBox &bounds,
                                       bool includeParents) const {
    return TileSystemIterator(*this, resolutionModel, bounds, includeParents);
}

TileSystemIterator TileSystem::iterate(const IResolutionModel &resolutionModel,
                                       bool includeParents) const {
    return iterate(resolutionModel, resolutionModel.getBounds());
}

} // namespace world
