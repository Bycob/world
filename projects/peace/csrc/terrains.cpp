#include "common.h"

#include <world/terrain.h>

using namespace world;

extern "C" {

/** \param data: Output of this function. Once used, should be deallocated
 * manually.
 * \param scale: Scale height according to the bounding box of the terrain. */
PEACE_EXPORT void readTerrain(TerrainPtr terrainPtr, float *data,
                              bool applyBbox) {
    auto *terrain = static_cast<Terrain *>(terrainPtr);
    double offset = 0, scale = 1;

    if (applyBbox) {
        const BoundingBox &bbox = terrain->getBoundingBox();
        offset = bbox.getLowerBound().z;
        scale = bbox.getDimensions().z;
    }
    HeightMapInputStream hmis(*terrain, offset, scale, HeightMapFormat::F32);
    hmis.read(reinterpret_cast<char *>(data), hmis.remaining());
}

PEACE_EXPORT int terrainGetResolution(TerrainPtr terrainPtr) {
    auto *terrain = static_cast<Terrain *>(terrainPtr);
    return terrain->getResolution();
}

PEACE_EXPORT MaterialPtr terrainGetMaterial(TerrainPtr terrainPtr) {
    auto *terrain = static_cast<Terrain *>(terrainPtr);
    return &terrain->getMaterial();
}

PEACE_EXPORT int terrainGetLayerCount(TerrainPtr terrainPtr) {
    auto *terrain = static_cast<Terrain *>(terrainPtr);
    auto &mat = terrain->getMaterial();
    const auto &customMaps = mat.getCustomMaps();

    int i = 0;
    while (customMaps.find("distribution" + std::to_string(i)) !=
           customMaps.end()) {
        ++i;
    }

    return i;
}

PEACE_EXPORT BBox terrainGetBBox(TerrainPtr terrainPtr) {
    auto *terrain = static_cast<Terrain *>(terrainPtr);
    const BoundingBox &bbox = terrain->getBoundingBox();
    auto lb = bbox.getLowerBound(), ub = bbox.getUpperBound();
    return {lb.x, lb.y, lb.z, ub.x, ub.y, ub.z};
}
}