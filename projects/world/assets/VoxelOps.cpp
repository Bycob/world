#include "VoxelOps.h"

#include "world/math/MathsHelper.h"

namespace world {

void VoxelOps::ball(VoxelField &voxels, const vec3d &origin, double radius,
                    double value) {
    double pixDistance = getPixelDistance(voxels);
    double smallRadius = radius - pixDistance;
    double bigRadius = radius + pixDistance;
    bigRadius *= bigRadius;
    smallRadius *= smallRadius;
    vec3u dims = voxels.dims();

    for (u32 z = 0; z < dims.z; ++z) {
        for (u32 y = 0; y < dims.y; ++y) {
            for (u32 x = 0; x < dims.x; ++x) {
                vec3d c = voxelIdToSpace(voxels, {x, y, z});
                double l = c.squaredLength(origin);
                // 1 -> in the ball
                double p =
                    clamp((l - bigRadius) / (smallRadius - bigRadius), 0, 1);
                voxels.at(x, y, z) = value * p + voxels.at(x, y, z) * (1 - p);
            }
        }
    }
}

double VoxelOps::getPixelDistance(const VoxelField &voxels) {
    vec3d dist = voxels.bbox().getDimensions() / (voxels.dims() - vec3d{1});
    return max(max(dist.x, dist.y), dist.z);
}

vec3d VoxelOps::voxelIdToSpace(const VoxelField &voxels, const vec3u &id) {
    vec3d p = id / (voxels.dims() - vec3d{1});
    const BoundingBox &bbox = voxels.bbox();
    return p * bbox.getDimensions() + bbox.getLowerBound();
}

} // namespace world
