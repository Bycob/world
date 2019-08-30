#include "InstancePool.h"

#include "world/math/RandomHelper.h"
#include "InstanceDistribution.h"

namespace world {

template <typename TGenerator, typename TDistribution>
inline void InstancePool<TGenerator, TDistribution>::collectSelf(
    ICollector &collector, const IResolutionModel &resolutionModel,
    const ExplorationContext &ctx) {

    // Add new species if required
    double totalArea = _chunksDecorated * _chunkArea;

    while (_generators.size() < _speciesDensity * totalArea &&
           _generators.size() < _minSpecies) {
        std::unique_ptr<TGenerator> newSpecies = std::make_unique<TGenerator>();
        _distribution.addGenerator(newSpecies->randomize());
        _generators.push_back(std::move(newSpecies));
    }

    // Collect generators objects
    _objects.clear();

    for (int id = 0; id < _generators.size(); ++id) {
        auto &generator = _generators.at(id);
        auto childCtx = ctx;
        childCtx.appendPrefix({NodeKeys::fromInt(id)});

        auto templates = generator->collectTemplates(collector, childCtx);
        _objects.emplace_back(templates);
    }
}

template <typename TGenerator, typename TDistribution>
inline void InstancePool<TGenerator, TDistribution>::decorate(Chunk &chunk) {
    // Each generator generates templates
    // Collect all templates
    // Distribute instances over the chunk

    if (chunk.getMinResolution() > _resolution ||
        chunk.getMaxResolution() < _resolution) {
        return;
    }

    ++_chunksDecorated;
    vec3d chunkDims = chunk.getSize();
    _chunkArea = chunkDims.x * chunkDims.y;

    // Distribution
    std::uniform_real_distribution<double> rotDistrib(0, M_PI * 2);
    auto &instance = chunk.addChild<Instance>();

    for (int genID = 0; genID < _generators.size(); ++genID) {
        auto &templates = _objects.at(genID);

        if (templates.empty()) {
            continue;
        }
        std::uniform_int_distribution<int> select(0, templates.size() - 1);
        auto positions = _distribution.getPositions(chunk, genID);

        for (auto &position : positions) {
            SceneNode object = templates[select(_rng)];

            // Apply random rotation and scaling
            object.setPosition(position);
            object.setRotation({0, 0, rotDistrib(_rng)});
            double scale = randScale(_rng, 1, 1.2);
            object.setScale({scale});

            instance.addNode(std::move(object));
        }
    }
}

template <typename TGenerator, typename TDistribution>
template <typename... Args>
TGenerator &InstancePool<TGenerator, TDistribution>::addGenerator(
    Args... args) {
    _generators.push_back(std::make_unique<TGenerator>(args...));
    // TODO Add custom HabitatFeatures
    _distribution.addGenerator(HabitatFeatures{});
    return *_generators.back();
}


inline void Instance::addNode(world::SceneNode node) {
    _nodes.push_back(std::move(node));
}


inline void Instance::collectSelf(ICollector &collector,
                                  const IResolutionModel &resolutionModel,
                                  const ExplorationContext &ctx) {
    if (collector.hasChannel<SceneNode>()) {
        auto &objChan = collector.getChannel<SceneNode>();

        for (int i = 0; i < _nodes.size(); ++i) {
            ItemKey key{std::to_string(i)};
            objChan.put(key, _nodes[i], ctx);
        }
    }
}

} // namespace world
