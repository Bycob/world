#ifndef WORLD_INSTANCE_POOL_H
#define WORLD_INSTANCE_POOL_H

#include "world/core/WorldConfig.h"

#include <random>
#include <vector>
#include <map>

#include "WorldKeys.h"
#include "WorldNode.h"
#include "IChunkDecorator.h"
#include "Chunk.h"
#include "InstanceDistribution.h"
#include "IInstanceGenerator.h"

namespace world {

class WORLDAPI_EXPORT InstancePool : public IChunkDecorator, public WorldNode {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    InstancePool();

    template <typename TDistribution, typename... Args>
    TDistribution &setDistribution(Args &&... args);

    DistributionBase &distribution() { return *_distribution; }

    void setResolution(double resolution);

    void collectSelf(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) override;

    void decorate(Chunk &chunk, const ExplorationContext &ctx) override;

    template <typename TGenerator> void setTemplateGenerator();

    template <typename TGenerator, typename... Args>
    TGenerator &addGenerator(Args... args);

    /** Export species meshes in a scene and habitat features in a json file.
     * \param avgSize Average size of the element, used to compute spacing
     * between objects in the scene. */
    void exportSpecies(const std::string &outputDir, double avgSize = 1,
                       double resolution = 10000);

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    std::unique_ptr<DistributionBase> _distribution;

    std::mt19937 _rng;
    std::unique_ptr<IInstanceGenerator> _templateGenerator;
    std::vector<std::unique_ptr<IInstanceGenerator>> _generators;
    std::vector<std::vector<Template>> _objects;
    u64 _chunksDecorated = 0;
    /// Internal field to remember the typical chunk area at the resolution of
    /// the pool
    double _chunkArea = 0;

    /// Typical resolution of an instance. Used to compute the chunk levels of
    /// the instances
    double _resolution = 20;
    /// The instance pool automatically adds new generators when expanding.
    /// This is the density of the species per km^2
    double _speciesDensity = 0.0004;
    /// Minimal species count
    u32 _minSpecies = 10;

    /// The factor by which the instances should be randomly scaled.
    /// 1 means no scaling. The more different to 1, the more scaled
    /// objects are.
    double _scaleFactor = 1.2;
};


class WORLDAPI_EXPORT Instance : public WorldNode {
public:
    Instance() = default;

    explicit Instance(Template tp) : _templates{std::move(tp)} {}

    void addNode(Template tp);

    size_t getNodeCount() const;

    void collectSelf(ICollector &collector,
                     const IResolutionModel &resolutionModel,
                     const ExplorationContext &ctx) override;

private:
    std::vector<Template> _templates;
};

} // namespace world

#include "InstancePool.inl"

#endif // WORLD_INSTANCE_POOL_H
