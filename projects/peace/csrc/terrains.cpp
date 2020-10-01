#include "common.h"

#include <world/core.h>
#include <world/terrain.h>

using namespace world;

extern "C" {

struct PEACE_EXPORT BBox {
    double xmin, ymin, zmin;
    double xmax, ymax, zmax;
};

/** \param data: Output of this function. Once used, should be deallocated
 * manually.
 * \param scale: Scale height according to the bounding box of the terrain. */
PEACE_EXPORT void readTerrain(TerrainPtr terrainPtr, double **data,
                              int *resolution, bool applyBbox) {
    auto *terrain = static_cast<Terrain *>(terrainPtr);
    *resolution = terrain->getResolution();
    double offset = 0, scale = 1;

    if (applyBbox) {
        const BoundingBox &bbox = terrain->getBoundingBox();
        offset = bbox.getLowerBound().z;
        scale = bbox.getDimensions().z;
    }
    HeightMapInputStream hmis(*terrain, offset, scale, HeightMapFormat::F64);
    *data = new double[hmis.remaining()];
    hmis.read(reinterpret_cast<char *>(*data), hmis.remaining());
}

PEACE_EXPORT BBox terrainGetBBox(TerrainPtr terrainPtr) {
    auto *terrain = static_cast<Terrain *>(terrainPtr);
    const BoundingBox &bbox = terrain->getBoundingBox();
    auto lb = bbox.getLowerBound(), ub = bbox.getUpperBound();
    return {lb.x, lb.y, lb.z, ub.x, ub.y, ub.z};
}

PEACE_EXPORT char *terrainGetDistribution(MaterialPtr materialPtr,
                                          int layerId) {
    auto *material = static_cast<Material *>(materialPtr);
    return strdup(
        material->getCustomMap("distribution" + std::to_string(layerId))
            .c_str());
}

PEACE_EXPORT char *terrainGetTexture(MaterialPtr materialPtr, int layerId) {
    auto *material = static_cast<Material *>(materialPtr);
    return strdup(
        material->getCustomMap("texture" + std::to_string(layerId)).c_str());
}

PEACE_EXPORT MaterialPtr terrainGetMaterial(TerrainPtr terrainPtr) {
    auto *terrain = static_cast<Terrain *>(terrainPtr);
    return &terrain->getMaterial();
}
}