#include <world/assets/ObjLoader.h>
#include "InstancePool.h"

#include "world/math/RandomHelper.h"
#include "InstanceDistribution.h"
#include "JsonUtils.h"
#include "Collector.h"
#include "ConstantResolution.h"
#include "IOUtil.h"

namespace world {

template <typename TGenerator, typename TDistribution>
inline void InstancePool<TGenerator, TDistribution>::collectSelf(
    ICollector &collector, const IResolutionModel &resolutionModel,
    const ExplorationContext &ctx) {

    // Add new species if required
    double totalArea = _chunksDecorated * _chunkArea / 1e6;

    while (_generators.size() < _speciesDensity * totalArea ||
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
    // Update species metadata
    vec3d chunkPos = chunk.getPosition3D();
    vec3d chunkDims = chunk.getSize();

    if (chunk.getMinResolution() <= 0.01 && chunk.getMaxResolution() > 0.01 &&
        chunkPos.z <= 0 && chunkPos.z + chunkDims.z > 0) {
        ++_chunksDecorated;
        _chunkArea = chunkDims.x * chunkDims.y;
    }

    if (chunk.getMinResolution() > _resolution ||
        chunk.getMaxResolution() < _resolution) {
        return;
    }

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

template <typename TGenerator, typename TDistribution>
void InstancePool<TGenerator, TDistribution>::exportSpecies(
    const std::string &outputDir, double avgSize) {
    world::createDirectories(outputDir);

    // Export different instance throught the scene
    Collector collector(CollectorPresets::SCENE);
    auto &nodeChan = collector.getChannel<SceneNode>();
    collectSelf(collector, ConstantResolution(0.1),
                ExplorationContext::getDefault());
    double sep = avgSize;

    for (size_t x = 0; x < _objects.size(); ++x) {
        auto &templates = _objects[x];

        for (size_t y = 0; y < templates.size(); ++y) {
            vec3d c{x * sep, y * sep, 0};
            SceneNode node = templates[y];
            node.setPosition(c);
            nodeChan.put({std::to_string(x), std::to_string(y)}, node);
        }
    }
    ObjLoader().write(collector.toScene(), outputDir + "/instances.obj");

    // Export habitat features
    const std::vector<HabitatFeatures> &habitats =
        _distribution.getHabitatFeatures();
    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);

    writer.StartObject();

    for (size_t i = 0; i < habitats.size(); ++i) {
        auto &habitat = habitats[i];
        writer.String(std::to_string(i));
        writer.StartObject();

        writer.String("alt_min");
        writer.Double(habitat._altitude.x);

        writer.String("alt_max");
        writer.Double(habitat._altitude.y);

        writer.String("density");
        writer.Double(habitat._density);

        writer.EndObject();
    }

    writer.EndObject();
    std::ofstream file(outputDir + "/habitats.json");
    file << buf.GetString();
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
