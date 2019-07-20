#ifndef WORLD_INSTANCE_POOL_H
#define WORLD_INSTANCE_POOL_H

#include "world/core/WorldConfig.h"

#include <random>
#include <vector>

#include "WorldKeys.h"
#include "WorldNode.h"
#include "IChunkDecorator.h"
#include "Chunk.h"

namespace world {

template <typename TGenerator>
class InstancePool : public IChunkDecorator, public WorldNode {
public:
    InstancePool(IEnvironment *env)
            : _env{env}, _rng(static_cast<u64>(time(NULL))) {}


    void setDensity(double density) { _density = density; }

    void collectSelf(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) override;

    void decorate(Chunk &chunk) override;

    template <typename... Args> TGenerator &addGenerator(Args... args);

private:
    IEnvironment *_env;

    std::mt19937 _rng;
    std::vector<std::unique_ptr<TGenerator>> _generators;
    std::vector<SceneNode> _objects;

    double _resolution = 20;
    // instance count per m^2
    double _density = 0.2;
};

class WORLDAPI_EXPORT Instance : public WorldNode {
public:
    Instance() {}

    Instance(SceneNode node) : _nodes{std::move(node)} {}

    void addNode(SceneNode node);

    void collectSelf(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) override;

private:
    std::vector<SceneNode> _nodes;
};

} // namespace world

#include "InstancePool.inl"

#endif // WORLD_INSTANCE_POOL_H
