#include <world/assets/ObjLoader.h>
#include "InstancePool.h"

#include "world/math/RandomHelper.h"
#include "InstanceDistribution.h"
#include "JsonUtils.h"
#include "Collector.h"
#include "ConstantResolution.h"
#include "IOUtil.h"
#include "world/assets/SceneNode.h"

// TODO move non template stuff in .cpp

namespace world {

inline InstancePool::InstancePool()
        : _distribution(std::make_unique<RandomDistribution>()),
          _rng(static_cast<u64>(time(NULL))) {}

template <typename TDistribution, typename... Args>
TDistribution &InstancePool::setDistribution(Args &&... args) {
    _distribution = std::make_unique<TDistribution>(args...);
    return static_cast<TDistribution &>(*_distribution);
}

inline void InstancePool::setResolution(double resolution) {
    _resolution = resolution;
}

inline void InstancePool::collectSelf(ICollector &collector,
                                      const IResolutionModel &resolutionModel,
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
        _distribution->addGenerator(newSpecies->randomize());
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

inline void InstancePool::decorate(Chunk &chunk,
                                   const ExplorationContext &ctx) {
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

    // std::cout << chunkPos << chunkDims << std::endl;

    // Distribution
    std::uniform_real_distribution<double> rotDistrib(0, M_PI * 2);
    auto &instance = chunk.addChild<Instance>();
    auto positions = _distribution->getPositions(chunk, ctx);

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
        double scale = randScale(_rng, 1, _scaleFactor);
        object._scale = {scale};

        instance.addNode(std::move(object));
    }

    if (instance.getNodeCount() == 0) {
        chunk.removeChild(instance);
    }
}

template <typename TGenerator> void InstancePool::setTemplateGenerator() {
    _templateGenerator = std::make_unique<TGenerator>();
}

template <typename TGenerator, typename... Args>
TGenerator &InstancePool::addGenerator(Args... args) {
    _generators.push_back(std::make_unique<TGenerator>(args...));
    // TODO Add custom HabitatFeatures
    _distribution->addGenerator(HabitatFeatures{});
    return static_cast<TGenerator &>(*_generators.back());
}

inline void InstancePool::write(WorldFile &wf) const {
    wf.addChild("distribution", _distribution->serializeSubclass());

    if (_templateGenerator) {
        wf.addChild("templateGenerator",
                    _templateGenerator->serializeSubclass());
    }

    wf.addArray("generators");

    for (auto &generator : _generators) {
        wf.addToArray("generators", generator->serializeSubclass());
    }

    wf.addDouble("resolution", _resolution);
    wf.addDouble("speciesDensity", _speciesDensity);
    wf.addUint("minSpecies", _minSpecies);
    wf.addDouble("scaleFactor", _scaleFactor);
}

inline void InstancePool::read(const WorldFile &wf) {
    _distribution.reset(
        readSubclass<DistributionBase>(wf.readChild("distribution")));

    if (wf.hasChild("templateGenerator")) {
        _templateGenerator = std::unique_ptr<IInstanceGenerator>(
            readSubclass<IInstanceGenerator>(
                wf.readChild("templateGenerator")));
    }

    for (auto it = wf.readArray("generators"); !it.end(); ++it) {
        _generators.emplace_back(std::unique_ptr<IInstanceGenerator>(
            readSubclass<IInstanceGenerator>(*it)));
    }

    wf.readDoubleOpt("resolution", _resolution);
    wf.readDoubleOpt("speciesDensity", _speciesDensity);
    wf.readUintOpt("minSpecies", _minSpecies);
    wf.readDoubleOpt("scaleFactor", _scaleFactor);

    _distribution->setResolution(_resolution);
}

inline void InstancePool::exportSpecies(const std::string &outputDir,
                                        double avgSize, double resolution) {
    world::createDirectories(outputDir);

    // Export different instance throught the scene
    Collector collector(CollectorPresets::SCENE);
    auto &nodeChan = collector.getChannel<SceneNode>();
    collectSelf(collector, ConstantResolution(resolution),
                ExplorationContext::getDefault());
    double sep = avgSize;

    for (size_t x = 0; x < _objects.size(); ++x) {
        auto &templates = _objects[x];

        for (size_t y = 0; y < templates.size(); ++y) {
            vec3d c{x * sep, y * sep, 0};
            auto item = templates[y].getAt(resolution);
            int i = 0;

            for (auto node : item->_nodes) {
                node.setPosition(c);
                nodeChan.put(ItemKey({std::to_string(x), std::to_string(y),
                                      std::to_string(i)}),
                             node);
                i++;
            }
        }
    }
    ObjLoader().write(collector.toScene(), outputDir + "/instances.obj");

    // Export habitat features
    WorldFile speciesTraits;
    speciesTraits.addArray("species");

    const std::vector<HabitatFeatures> &habitats =
        _distribution->getHabitatFeatures();

    for (size_t i = 0; i < habitats.size(); ++i) {
        WorldFile instance;
        instance.addChild("habitat", world::serialize(habitats[i]));
        instance.addChild("features", _generators[i]->serialize());

        speciesTraits.addToArray("species", std::move(instance));
    }

    speciesTraits.save(outputDir + "/habitats.json");
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
                    objChan.put({key,
                                 {std::to_string(j) + "." +
                                  std::to_string(nodes->_minRes)}},
                                node, ctx);
                    ++j;
                }
            }
        }
    }
}

} // namespace world
