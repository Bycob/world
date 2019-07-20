#include "InstancePool.h"

#include "world/math/RandomHelper.h"

namespace world {


template <typename TGenerator>
inline void InstancePool<TGenerator>::collectSelf(
    ICollector &collector, const IResolutionModel &resolutionModel,
    const ExplorationContext &ctx) {

    int id = 0;

    for (auto &generator : _generators) {
        auto childCtx = ctx;
        childCtx.appendPrefix({NodeKeys::fromInt(id)});
        auto templates = generator->collectTemplates(collector, childCtx);
        _objects.insert(_objects.end(), templates.begin(), templates.end());

        ++id;
    }
}

template <typename TGenerator>
inline void InstancePool<TGenerator>::decorate(Chunk &chunk) {
    // Each generator generates templates
    // Collect all templates
    // Distribute instances over the chunk

    if (chunk.getMinResolution() > _resolution ||
        chunk.getMaxResolution() < _resolution) {
        return;
    }

    if (_objects.empty()) {
        return;
    }

    const vec3d chunkPos = chunk.getPosition3D();
    const vec3d chunkDims = chunk.getSize();

    // Distribution
    double chunkArea = chunkDims.x * chunkDims.y;
    int instanceCount = static_cast<int>(floor(chunkArea * _density));
    std::uniform_real_distribution<double> posDistrib(0, 1);
    std::uniform_real_distribution<double> rotDistrib(0, M_PI * 2);
    std::uniform_int_distribution<int> select(0, _objects.size() - 1);

    auto &instance = chunk.addChild<Instance>();

    for (int i = 0; i < instanceCount; ++i) {
        double x = posDistrib(_rng) * chunkDims.x;
        double y = posDistrib(_rng) * chunkDims.y;
        vec3d position = _env->findNearestFreePoint(
                             chunkPos + vec3d{x, y, -3000}, vec3d{0, 0, 1},
                             _resolution, ExplorationContext::getDefault()) -
                         chunkPos;

        if (position.z < 0 || position.z >= chunkDims.z) {
            continue;
        }

        // Pick random template
        int templateId = select(_rng);
        SceneNode object = _objects.at(templateId);

        // Apply random rotation and scaling
        object.setPosition(position);
        object.setRotation({0, 0, rotDistrib(_rng)});
        double scale = randScale(_rng, 1, 1.2);
        object.setScale({scale});

        instance.addNode(std::move(object));
    }
}

template <typename TGenerator>
template <typename... Args>
TGenerator &InstancePool<TGenerator>::addGenerator(Args... args) {
    _generators.push_back(std::make_unique<TGenerator>(args...));
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
