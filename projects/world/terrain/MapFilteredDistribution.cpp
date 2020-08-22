#include "MapFilteredDistribution.h"

#include "world/core/SeedDistribution.h"

namespace world {
WORLD_REGISTER_TEMPLATE_CHILD_CLASS(
    DistributionBase, MapFilteredDistribution, RandomDistribution,
    "MapFilteredDistribution<RandomDistribution>")
WORLD_REGISTER_TEMPLATE_CHILD_CLASS(DistributionBase, MapFilteredDistribution,
                                    SeedDistribution,
                                    "MapFilteredDistribution<SeedDistribution>")
} // namespace world