#ifndef WORLD_FOREST_LAYER_H
#define WORLD_FOREST_LAYER_H

#include "world/core/WorldConfig.h"

#include <random>

#include "world/flat/FlatWorld.h"

namespace world {

class WORLDAPI_EXPORT ForestLayer : public FlatWorldDecorator {
public:
    ForestLayer();

    void decorate(FlatWorld &world, const WorldZone &zone) override;

private:
    std::mt19937 _rng;

    /// Maximum possible density of trees, in tree.km^-2
    double _maxDensity = 5000;
    // 20000 is better, but needs to be optimized both in memory and mesh
    // complexity

    double getDensityAtAltitude(double altitude);
};

} // namespace world

#endif // WORLD_FOREST_LAYER_H
