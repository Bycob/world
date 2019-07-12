#pragma once

#include "world/core/WorldConfig.h"

#include "VoxelGrid.h"

namespace world {

class WORLDAPI_EXPORT VoxelOps {
public:
    VoxelOps() = delete;

    static void ball(VoxelField &voxels, const vec3d &origin, double radius,
                     double value);

    static double getPixelDistance(const VoxelField &voxels);

    static vec3d voxelIdToSpace(const VoxelField &voxels, const vec3u &id);
};
} // namespace world
