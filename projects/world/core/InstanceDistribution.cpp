#include "InstanceDistribution.h"

namespace world {

WORLD_REGISTER_BASE_CLASS(DistributionBase);

WORLD_REGISTER_CHILD_CLASS(DistributionBase, RandomDistribution,
                           "RandomDistribution")

} // namespace world