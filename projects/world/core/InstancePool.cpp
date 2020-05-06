#include "InstancePool.h"
#include "SeedDistribution.h"

namespace world {
template class WORLDAPI_EXPORT InstancePool<RandomDistribution>;
template class WORLDAPI_EXPORT InstancePool<SeedDistribution>;

WORLD_REGISTER_TEMPLATE_CHILD_CLASS(IChunkDecorator, InstancePool,
                                    RandomDistribution,
                                    "InstancePool_RandomDistribution")
WORLD_REGISTER_TEMPLATE_CHILD_CLASS(IChunkDecorator, InstancePool,
                                    SeedDistribution,
                                    "InstancePool_SeedDistribution")
} // namespace world