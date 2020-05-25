#pragma once

#include "world/core/WorldConfig.h"

#include <memory>

#include "world/core/IResolutionModel.h"
#include "world/core/WorldNode.h"
#include "world/core/IInstanceGenerator.h"
#include "world/core/ObjectNode.h"
#include "world/core/SpriteGrid.h"
#include "world/assets/Mesh.h"
#include "world/assets/Material.h"
#include "ITreeWorker.h"
#include "TreeSkeletton.h"

namespace world {

class TreeInstance : public ObjectInstance {
public:
    Tree &_tree;

    vec3d _pos;
    TreeSkeletton _skeletton;

    /// Resolution at which the instance is currently generated.
    double _resolution = 0;


    TreeInstance(Tree &tree, vec3d pos);

    Mesh &simpleTrunk();
    Mesh &simpleLeaves();
    Mesh &trunkMesh();
    Mesh &leavesMesh();

    Material &trunkMaterial();
    Image &trunkTexture();

    void reset();
};

class PTree;

class WORLDAPI_EXPORT Tree : public WorldNode, public IInstanceGenerator {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    Tree();

    ~Tree() override;

    void addTree(vec3d pos = {});

    Image &getLeavesTexture();

    const SpriteGrid &getLeavesGrid();

    TreeInstance &getTreeInstance(int i);

    void setup(const Tree &model);

    template <typename T, typename... Args> T &addWorker(Args &&... args);

    void collect(ICollector &collector, const IResolutionModel &explorer,
                 const ExplorationContext &ctx) override;

    std::vector<Template> collectTemplates(ICollector &collector,
                                           const ExplorationContext &ctx,
                                           double maxRes) override;

    HabitatFeatures randomize() override;

    Tree *newInstance() override { return new Tree(); }

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    PTree *_internal;


    void addWorkerInternal(ITreeWorker *worker);

    Template collectTree(TreeInstance &instance, ICollector &collector,
                         const ExplorationContext &ctx, double res);

    void generateSelf(double resolution);

    void generate(TreeInstance &instance, double resolution);

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
