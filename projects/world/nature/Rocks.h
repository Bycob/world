#ifndef WORLD_NATURE_ROCKS_H
#define WORLD_NATURE_ROCKS_H

#include "world/core/WorldConfig.h"

#include <random>

#include "world/assets/Object3D.h"
#include "world/core/WorldNode.h"

namespace world {

class WORLDAPI_EXPORT Rocks : public WorldNode {
public:
    Rocks();

    void addRock(const vec3d &location);

    void collectSelf(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) override;

private:
    std::mt19937_64 _rng;
    std::vector<Object3D> _objects;

    double _radius = 1;
    // number of cuts
    u32 _roughness = 10;
    // flatness of the cuts
    double _flatness = 2;


    void generateMesh(Mesh &mesh);
};

} // namespace world

#endif // WORLD_NATURE_ROCKS_H
