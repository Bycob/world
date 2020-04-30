#include <world/assets/ObjLoader.h>
#include "InstancePool.h"

#include "world/math/RandomHelper.h"
#include "InstanceDistribution.h"
#include "JsonUtils.h"
#include "Collector.h"
#include "ConstantResolution.h"
#include "IOUtil.h"
#include "world/assets/SceneNode.h"

namespace world {

template <typename TDistribution>
void InstancePool<TDistribution>::setResolution(double resolution) {
    _resolution = resolution;
}

template <typename TDistribution>
inline void InstancePool<TDistribution>::collectSelf(
    ICollector &collector, const IResolutionModel &resolutionModel,
    const ExplorationContext &ctx) {

    // Add new species if required
    double totalArea = _chunksDecorated * _chunkArea / 1e6;

    while (_generators.size() < _speciesDensity * totalArea ||
           _generators.size() < _minSpecies) {

        if (!_templateGenerator) {
            throw std::runtime_error(
                "[InstancePool] No template, generation can not be performed");
        }

        std::unique_ptr<IInstanceGenerator> newSpecies =
            std::unique_ptr<IInstanceGenerator>(
                _templateGenerator->newInstance());
        _distribution.addGenerator(newSpecies->randomize());
        _generators.push_back(std::move(newSpecies));
    }

    // Collect generators objects
    _objects.clear();

    for (int id = 0; id < _generators.size(); ++id) {
        auto &generator = _generators.at(id);
        auto childCtx = ctx;
        childCtx.appendPrefix({NodeKeys::fromInt(id)});

        // TODO collect at the correct resolution, then update all templates of
        // the instance. At the moment the max res is set to 10000, so we
        // collect (hopefully) all the LODs.
        auto templates =
            generator->collectTemplates(collector, childCtx, 10000);
        _objects.emplace_back(templates);
    }
}

template <typename TDistribution>
inline void InstancePool<TDistribution>::decorate(
    Chunk &chunk, const ExplorationContext &ctx) {
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
    auto positions = _distribution.getPositions(chunk, ctx);

    for (auto &position : positions) {
        auto &templates = _objects.at(position._genID);

        if (templates.empty()) {
            continue;
        }

        std::uniform_int_distribution<int> select(0, templates.size() - 1);
        Template object = templates[select(_rng)];

        // Apply random rotation and scaling
        object._position = position._pos;
        object._rotation = {0, 0, rotDistrib(_rng)};
        double scale = randScale(_rng, 1, 1.2);
        object._scale = {scale};

        instance.addNode(std::move(object));
    }

    if (instance.getNodeCount() == 0) {
        chunk.removeChild(instance);
    }
}

template <typename TDistribution>
template <typename TGenerator>
void InstancePool<TDistribution>::setTemplateGenerator() {
    _templateGenerator = std::make_unique<TGenerator>();
}

template <typename TDistribution>
template <typename TGenerator, typename... Args>
TGenerator &InstancePool<TDistribution>::addGenerator(Args... args) {
    _generators.push_back(std::make_unique<TGenerator>(args...));
    // TODO Add custom HabitatFeatures
    _distribution.addGenerator(HabitatFeatures{});
    return static_cast<TGenerator &>(*_generators.back());
}

template <typename TDistribution>
void InstancePool<TDistribution>::write(WorldFile &wf) const {
    wf.addChild("distribution", _distribution.serialize());

    if (_templateGenerator) {
        wf.addChild("templateGenerator",
                    _templateGenerator->serializeSubclass());
    }

    wf.addArray("generators");

    for (auto &generator : _generators) {
        wf.addToArray("generators", generator->serializeSubclass());
    }
}

template <typename TDistribution>
void InstancePool<TDistribution>::read(const WorldFile &wf) {
    _distribution.read(wf.readChild("distribution"));

    if (wf.hasChild("templateGenerator")) {
        _templateGenerator = std::unique_ptr<IInstanceGenerator>(
            readSubclass<IInstanceGenerator>(
                wf.readChild("templateGenerator")));
    }

    for (auto it = wf.readArray("generators"); !it.end(); ++it) {
        _generators.emplace_back(std::unique_ptr<IInstanceGenerator>(
            readSubclass<IInstanceGenerator>(*it)));
    }
}

template <typename TDistribution>
void InstancePool<TDistribution>::exportSpecies(const std::string &outputDir,
                                                double avgSize) {
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
            SceneNode node = templates[y].getDefaultNode();
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


// #### Instance

inline void Instance::addNode(Template tp) {
    _templates.push_back(std::move(tp));
}

inline size_t Instance::getNodeCount() const { return _templates.size(); }

inline void Instance::collectSelf(ICollector &collector,
                                  const IResolutionModel &resolutionModel,
                                  const ExplorationContext &ctx) {
    if (collector.hasChannel<SceneNode>()) {
        auto &objChan = collector.getChannel<SceneNode>();

        for (size_t i = 0; i < _templates.size(); ++i) {
            ItemKey key{std::to_string(i)};
            auto &tp = _templates[i];

            // Get the nodes corresponding to the right resolution
            double resolution =
                resolutionModel.getResolutionAt(tp._position, ctx);
            auto *nodes = tp.getAt(resolution);

            if (nodes != nullptr) {
                // Add every node of the resolution level to the collector
                int j = 0;

                for (SceneNode node : nodes->_nodes) {
                    // Update each object's transform based on the global one
                    node.setPosition(node.getPosition() * tp._scale +
                                     tp._position);
                    // TODO update position based on rotation
                    node.setRotation(tp._rotation);
                    node.setScale(node.getScale() * tp._scale);
                    objChan.put({key, std::to_string(j) + "." +
                                          std::to_string(nodes->_minRes)},
                                node, ctx);
                    ++j;
                }
            }
        }
    }
}

} // namespace world
