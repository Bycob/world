#ifndef WORLD_NATURE_ROCKS_H
#define WORLD_NATURE_ROCKS_H

#include "world/core/WorldConfig.h"

#include <random>

#include "world/assets/SceneNode.h"
#include "world/core/WorldNode.h"

namespace world {

class WORLDAPI_EXPORT Rocks : public WorldNode {
public:
    Rocks();

    void addRock(const vec3d &position);

    void setRadius(double radius) { _radius = radius; }

    std::vector<SceneNode> collectTemplates(ICollector &collector,
                                            const ExplorationContext &ctx);

    void collectSelf(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) override;

private:
    struct Rock {
        vec3d position;
        Mesh mesh;
    };
    std::mt19937_64 _rng;
    std::vector<Rock> _rocks;

    double _radius = 1;
    // number of cuts
    u32 _roughness = 10;
    // flatness of the cuts
    double _flatness = 2;


    void generateMesh(Mesh &mesh);
};

} // namespace world

#endif // WORLD_NATURE_ROCKS_H
