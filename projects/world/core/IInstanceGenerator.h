#ifndef WORLD_IINSTANCEGENERATOR_H
#define WORLD_IINSTANCEGENERATOR_H

#include "world/core/WorldConfig.h"

#include "InstanceDistribution.h"
#include "ICollector.h"

namespace world {

// TODO Force every instance generator to inherit this class
// (by specifying TGenerator in the InstancePool)
class WORLDAPI_EXPORT IInstanceGenerator {
public:
    std::vector<SceneNode> collectTemplates(ICollector &collector,
                                            const ExplorationContext &ctx);

    HabitatFeatures randomize();

private:
};

} // namespace world

#endif // WORLD_IINSTANCEGENERATOR_H
