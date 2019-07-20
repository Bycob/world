#include "Rocks.h"

#include "world/math/RandomHelper.h"
#include "world/assets/VoxelGrid.h"
#include "world/assets/VoxelOps.h"

namespace world {
Rocks::Rocks() : _rng(time(NULL)) {}

void Rocks::addRock(const vec3d &position) {
    _rocks.emplace_back();
    generateMesh(_rocks.back().mesh);
    _rocks.back().position = position;
}

std::vector<SceneNode> Rocks::collectTemplates(ICollector &collector,
                                               const ExplorationContext &ctx) {
    std::vector<SceneNode> nodes;

    for (int i = 0; i < _rocks.size(); ++i) {
        ItemKey key{NodeKeys::fromInt(i)};
        ItemKey matKey;

        if (collector.hasChannel<Mesh>()) {
            auto &meshChan = collector.getChannel<Mesh>();
            meshChan.put(key, _rocks[i].mesh, ctx);

            if (collector.hasChannel<Material>()) {
                auto &matChan = collector.getChannel<Material>();

                Material rockMat("rock");
                rockMat.setKd(0.6, 0.6, 0.6);
                matChan.put(matKey = key, rockMat, ctx);
            }

            auto node = ctx.createNode(key, matKey);
            node.setPosition(_rocks[i].position);
            nodes.push_back(node);
        }
    }

    return nodes;
}

void Rocks::collectSelf(ICollector &collector,
                        const IResolutionModel &resolutionModel,
                        const ExplorationContext &ctx) {

    if (collector.hasChannel<SceneNode>()) {
        auto &objChan = collector.getChannel<SceneNode>();
        auto &meshChan = collector.getChannel<Mesh>();

        int i = 0;
        for (auto &rock : _rocks) {
            ItemKey key{std::to_string(i)};
            meshChan.put(key, rock.mesh, ctx);

            SceneNode obj(ctx.mutateKey(key).str());
            obj.setPosition(rock.position);
            objChan.put(key, obj, ctx);
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
