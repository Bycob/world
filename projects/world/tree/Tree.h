#pragma once

#include "world/core/WorldConfig.h"

#include <memory>

#include "world/core/IResolutionModel.h"
#include "world/core/WorldNode.h"
#include "world/core/IInstanceGenerator.h"
#include "world/assets/Mesh.h"
#include "world/assets/Material.h"
#include "ITreeWorker.h"
#include "TreeSkeletton.h"

namespace world {

class PTree;

class WORLDAPI_EXPORT Tree : public WorldNode, public IInstanceGenerator {
public:
    Tree();

    ~Tree() override;

    void setup(const Tree &model);

    template <typename T, typename... Args> T &addWorker(Args &&... args);

    const TreeSkeletton &getSkeletton() const;

    TreeSkeletton &getSkeletton();

    const Mesh &getTrunkMesh() const;

    Mesh &getTrunkMesh();

    const Mesh &getLeavesMesh() const;

    Mesh &leavesMesh();

    void collect(ICollector &collector, const IResolutionModel &explorer,
                 const ExplorationContext &ctx) override;

    std::vector<Template> collectTemplates(ICollector &collector,
                                           const ExplorationContext &ctx,
                                           double maxRes);

    HabitatFeatures randomize();

public:
    Mesh _simpleTrunk;
    Mesh _simpleLeaves;

    void generateSimpleMeshes();

private:
    PTree *_internal;

    TreeSkeletton _skeletton;
    Mesh _trunkMesh;
    Mesh _leavesMesh;
    Material _trunkMaterial;

    bool _generated = false;


    void addWorkerInternal(ITreeWorker *worker);

    void generateBase();

    /** Ungenerate the tree */
    void reset();

    friend class TrunkGenerator;
};

template <typename T, typename... Args> T &Tree::addWorker(Args &&... args) {
    T *worker = new T(args...);
    addWorkerInternal(worker);
    return *worker;
}
} // namespace world
