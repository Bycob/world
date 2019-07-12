#include "Rocks.h"

#include "world/math/RandomHelper.h"
#include "world/assets/VoxelGrid.h"
#include "world/assets/VoxelOps.h"

namespace world {
Rocks::Rocks() : _rng(time(NULL)) {}

void Rocks::addRock(const vec3d &location) {
    Mesh mesh;
    generateMesh(mesh);
    _objects.emplace_back(mesh);
    _objects.back().setPosition(location);
}

void Rocks::collectSelf(ICollector &collector,
                        const IResolutionModel &resolutionModel,
                        const ExplorationContext &ctx) {

    if (collector.hasChannel<Object3D>()) {
        auto &objChan = collector.getChannel<Object3D>();

        int i = 0;
        for (auto &obj : _objects) {
            objChan.put({std::to_string(i)}, obj, ctx);
            ++i;
        }
    }
}

void Rocks::generateMesh(Mesh &mesh) {
    VoxelField voxels({12, 12, 12}, -1);
    voxels.bbox().reset({-_radius * 1.2}, {_radius * 1.2});

    // Add matter
    VoxelOps::ball(voxels, {0}, _radius, 1);

    // Remove matter
    std::uniform_real_distribution<double> thetaDistr(0, M_PI * 2);
    std::uniform_real_distribution<double> phiDistr(0, M_PI);

    for (u32 i = 0; i < _roughness; ++i) {
        double theta = thetaDistr(_rng);
        double phi = phiDistr(_rng);
        vec3d dir{cos(theta) * sin(phi), std::sin(theta) * sin(phi), cos(phi)};

        double distance = randScale(_rng, _flatness);
        double radius = distance - _radius * randScale(_rng, 0.65);

        VoxelOps::ball(voxels, dir * distance, radius, -1);
    }

    voxels.fillMesh(mesh);
}
} // namespace world
