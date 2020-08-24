#include "Tree.h"

#include <vector>

#include "world/core/IResolutionModel.h"
#include "world/assets/SceneNode.h"
#include "world/assets/MeshOps.h"
#include "TreeSkelettonEQaTWorker.h"
#include "TreeSkelettonHoGBasedWorker.h"
#include "TrunkGenerator.h"
#include "LeavesGenerator.h"
#include "world/math/RandomHelper.h"
#include "QuickLeaves.h"
#include "TreeBillboardWorker.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(WorldNode, Tree, "Tree");
WORLD_SECOND_REGISTER_CHILD_CLASS(IInstanceGenerator, Tree, "Tree")


class PTree {
public:
    std::vector<std::unique_ptr<ITreeWorker>> _workers;

    std::vector<std::unique_ptr<TreeInstance>> _instances;
    BoundingBox _bbox;

    // Common assets
    Image _trunkTexture;
    Image _leavesTexture;
    SpriteGrid _grid;

    // lods: lists of generated resolution
    std::vector<double> _singleMeshLod;
    std::vector<double> _twoMeshesLod;

    Color4d _defaultTrunkColor = {0.3, 0.17, 0.13};
    Color4d _defaultLeavesColor = {0.4, 0.9, 0.4};

    /// Resolution at which the instance is currently generated.
    double _resolution = 0;


    PTree()
            : _trunkTexture(1, 1, ImageType::RGBA),
              _leavesTexture(1, 1, ImageType::RGBA),
              _grid(4), _singleMeshLod{2}, _twoMeshesLod{15} {

        _trunkTexture.rgba(0, 0) = _defaultTrunkColor;
        _leavesTexture.rgba(0, 0) = _defaultLeavesColor;
    }
};


// ===== TreeInstance

TreeInstance::TreeInstance(Tree &tree, vec3d pos)
        : ObjectInstance(pos), _tree(tree) {

    for (auto res : tree._internal->_singleMeshLod) {
        auto &lod = addLod(res, 1);

        Material &treeMat = lod.addMaterial();
        treeMat.setMapKd(lod.getId(0));
        treeMat.setTransparent(true);
        lod.getNode(0).setMaterial(treeMat);

        lod.addTexture(1, 1, ImageType::RGBA);
    }

    for (auto res : tree._internal->_twoMeshesLod) {
        auto &lod = addLod(res, 2);

        lod.getNode(0).setMaterialID("trunk");
        lod.getNode(1).setMaterialID("leaves");
    }
}

Mesh &TreeInstance::trunkMesh(double res) {
    return getLodByResolution(res).getMesh(0);
}

Mesh &TreeInstance::leavesMesh(double res) {
    return getLodByResolution(res).getMesh(1);
}

Material &TreeInstance::overrideMaterial(int id, double resolution) {
    auto &lod = getLodByResolution(resolution);

    while (lod.getMaterialCount() <= id) {
        lod.addMaterial();
    }

    auto &mat = lod.getMaterial(id);
    lod.getNode(id).setMaterial(mat);

    if (id == 1)
        mat.setTransparent(true);

    return mat;
}

Image &TreeInstance::overrideTexture(int id, double resolution) {
    auto &lod = getLodByResolution(resolution);

    while (lod.getTextureCount() <= id) {
        lod.addTexture();
    }

    overrideMaterial(id, resolution).setMapKd(lod.getId(id));
    return lod.getTexture(id);
}

void TreeInstance::reset() {
    _resolution = 0;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            getLod(i).getMesh(j) = Mesh();
        }
    }
}


// ===== Tree

Tree::Tree() : _internal(new PTree()) {}

Tree::~Tree() { delete _internal; }

void Tree::addTree(vec3d pos) {
    _internal->_instances.emplace_back(
        std::make_unique<TreeInstance>(*this, pos));

    if (_internal->_instances.size() == 1) {
        _internal->_bbox.reset(pos);
    } else {
        _internal->_bbox.addPoint(pos);
    }
}

Image &Tree::getLeavesTexture() { return _internal->_leavesTexture; }

Image &Tree::getTrunkTexture() { return _internal->_trunkTexture; }

const SpriteGrid &Tree::getLeavesGrid() { return _internal->_grid; }

bool Tree::isTwoMeshes(double resolution) const {
    return resolution >= _internal->_twoMeshesLod.at(0);
}

TreeInstance &Tree::getTreeInstance(int i) {
    return *_internal->_instances.at(i);
}

void Tree::setup(const Tree &model) {
    _internal->_workers.clear();

    for (auto &worker : model._internal->_workers) {
        addWorkerInternal(worker->clone());
    }

    for (auto &ti : model._internal->_instances) {
        addTree(ti->getPosition());
    }
}

void Tree::collect(ICollector &collector,
                   const IResolutionModel &resolutionModel,
                   const ExplorationContext &ctx) {

    const double minRes = _internal->_singleMeshLod.at(0);
    auto allRes = resolutionModel.getMaxResolutionIn(_internal->_bbox, ctx);

    // All the trees are too far to be seen
    if (allRes < minRes)
        return;

    if (collector.hasChannel<SceneNode>()) {
        auto &objChan = collector.getChannel<SceneNode>();
        int tpCount = 0;

        for (auto &ti : _internal->_instances) {
            ++tpCount;
            vec3d realPos = ctx.getEnvironment().findNearestFreePoint(
                ti->getPosition(), {0, 0, 1}, minRes, ctx);
            double resolution = resolutionModel.getResolutionAt(realPos, ctx);

            // This particular tree is too far to be seen
            if (resolution < minRes)
                continue;

            Template tp = collectInstance(*ti, collector, ctx, resolution);
            auto *item = tp.getAt(resolution);

            if (item != nullptr) {
                int i = 0;

                for (SceneNode node : item->_nodes) {
                    ItemKey key{std::to_string(tpCount) + "." +
                                std::to_string(i) + "." +
                                std::to_string(item->_minRes)};

                    node.setPosition(
                        node.getPosition() +
                        ctx.getEnvironment().findNearestFreePoint(
                            tp._position, {0, 0, 1}, item->_minRes, ctx));

                    objChan.put(key, node, ctx);
                    ++i;
                }
            }
        }
    }
}

std::vector<Template> Tree::collectTemplates(ICollector &collector,
                                             const ExplorationContext &ctx,
                                             double maxRes) {
    std::vector<Template> templates;
    int i = 0;

    for (auto &ti : _internal->_instances) {
        auto childCtx = ctx;
        childCtx.appendPrefix(std::to_string(i));

        templates.push_back(collectInstance(*ti, collector, childCtx, maxRes));

        ++i;
    }

    return templates;
}

HabitatFeatures Tree::randomize() {
    reset();
    _internal->_workers.clear();

    auto &skeletton = addWorker<TreeSkelettonHoGBasedWorker>();
    skeletton.randomize();

    double treeWeight = skeletton.getStartWeight();
    double treeHeight = pow(treeWeight, 1. / 3.) * 3;
    double treeArea = pow(treeWeight, 2. / 3.) * 3;

    // Colors
    std::mt19937 rng(std::random_device{}());
    Color4d trunkColor = jitter(rng, {0.3, 0.17, 0.13}, 0.1);
    Color4d leavesColor = jitter(rng, {0.4, 0.9, 0.4}, 0.1);

    _internal->_trunkTexture.rgba(0, 0) = trunkColor;
    _internal->_leavesTexture.rgba(0, 0) = leavesColor;

    // Trunk and leaves
    addWorker<TreeBillboardWorker>(trunkColor, leavesColor, treeHeight);
    addWorker<TrunkGenerator>(9);
    // addWorker<QuickLeaves>(6, leavesColor);
    addWorker<LeavesGenerator>(0.2, 0.02);

    // Add vulkan modules for texturing if vulkan is available
    try {
        // Use the serialization API to avoid linking with vkworld
        WorldFile trunkTex;
        trunkTex.addString("type", "VkwTrunkTexture");
        trunkTex.addStruct("color", trunkColor);
        addWorkerInternal(readSubclass<ITreeWorker>(trunkTex));

        WorldFile leavesTex;
        leavesTex.addString("type", "VkwLeafTexture");
        leavesTex.addStruct("mainColor", leavesColor);
        addWorkerInternal(readSubclass<ITreeWorker>(leavesTex));
    } catch (...) {
        // no vulkan modules
        std::cout << "[Warning] Vulkan modules are not available" << std::endl;
    }

    while (_internal->_instances.size() < 5) {
        addTree();
    }

    // Habitat
    HabitatFeatures habitat;
    double minSlope = 0.5;
    habitat._slope = {
        0, M_PI_2 * (minSlope + (1 - minSlope) * exp(-treeHeight * 0.2))};
    habitat._density = 1. / (treeArea + treeHeight);

    auto altStart = exponentialDistribFromMedian(1.25 / treeHeight);
    std::uniform_real_distribution<double> altRange(200, 1000);
    double lowAlt = altStart(rng) * 1100 - 100;
    habitat._altitude = {lowAlt, lowAlt + altRange(rng)};

    habitat._sea = false;

    return habitat;
}

void Tree::write(WorldFile &wf) const {
    WorldNode::write(wf);
    wf.addArray("workers");

    for (auto &worker : _internal->_workers) {
        wf.addToArray("workers", worker->serializeSubclass());
    }

    // TODO need serialization of arrays of double
    /*wf.addArray("singleMeshLod");

    for (double d : _internal->_singleMeshLod) {
        wf.addToArray("singleMeshLod", world::serialize(d));
    }

    wf.addArray("twoMeshesLod");

    for (double d : _internal->_twoMeshesLod) {
        wf.addToArray("twoMeshesLod", world::serialize(d));
    }*/
}

void Tree::read(const WorldFile &wf) {
    WorldNode::read(wf);

    for (auto it = wf.readArray("workers"); !it.end(); ++it) {
        _internal->_workers.emplace_back(readSubclass<ITreeWorker>(*it));
    }
}

Template Tree::collectInstance(TreeInstance &ti, ICollector &collector,
                               const ExplorationContext &ctx, double res) {

    // Generate
    for (int i = 0; i < ti.getLodCount(); ++i) {
        double lodRes = ti.getLod(i)._resolution;

        if (res > lodRes)
            generate(ti, lodRes);
    }

    // Collect
    // TODO collectCommon is ran more than once, because reference are broken by
    // the change of context
    // Fix: after key API is reworked, maybe we'll have a ".." equivalent?
    collectCommon(collector, ctx, res);
    Template tp = ti.collect(collector, ctx, res);
    return tp;
}

void Tree::collectCommon(ICollector &collector, const ExplorationContext &ctx,
                         double res) {

    if (collector.hasChannel<Material>()) {
        auto &materialsChannel = collector.getChannel<Material>();

        Material leavesMat("leaves");
        auto &leavesTex = _internal->_leavesTexture;

        Material trunkMat("trunk");
        auto &trunkTex = _internal->_trunkTexture;

        if (collector.hasChannel<Image>()) {
            auto &texChannel = collector.getChannel<Image>();

            leavesMat.setKd(1.0, 1.0, 1.0);
            leavesMat.setMapKd(ctx({"leaves"}).str());
            leavesMat.setTransparent(true);
            texChannel.put({"leaves"}, leavesTex, ctx);

            trunkMat.setKd(1.0, 1.0, 1.0);
            trunkMat.setMapKd(ctx({"trunk"}).str());
            texChannel.put({"trunk"}, trunkTex, ctx);
        } else {
            leavesMat.setKd(0.4, 0.9, 0.4);
            trunkMat.setKd(0.3, 0.17, 0.13);
        }

        materialsChannel.put({"leaves"}, leavesMat, ctx);
        materialsChannel.put({"trunk"}, trunkMat, ctx);
    }
}

void Tree::generateSelf(double resolution) {
    if (_internal->_resolution >= resolution) {
        return;
    }

    for (auto &worker : _internal->_workers) {
        worker->processTree(*this, resolution);
    }

    _internal->_resolution = resolution;
}

void Tree::generate(TreeInstance &instance, double resolution) {
    generateSelf(resolution);

    if (instance._resolution >= resolution) {
        return;
    }

    for (auto &worker : _internal->_workers) {
        worker->processInstance(instance, resolution);
    }
    // u32 total = instance._trunkMesh.getVerticesCount() +
    // instance._leavesMesh.getVerticesCount(); std::cout << total << " "  << u
    // / total << std::endl; std::cout << (u += total * sizeof(Vertex)) <<
    // std::endl;

    instance._resolution = resolution;
}

void Tree::reset() {
    for (auto &ti : _internal->_instances) {
        ti->reset();
    }

    _internal->_resolution = 0;

    // Resetting images
    _internal->_trunkTexture = Image(1, 1, ImageType::RGBA);
    _internal->_trunkTexture.rgba(0, 0) = _internal->_defaultTrunkColor;

    _internal->_leavesTexture = Image(1, 1, ImageType::RGBA);
    _internal->_leavesTexture.rgba(0, 0) = _internal->_defaultLeavesColor;
}

void Tree::addWorkerInternal(ITreeWorker *worker) {
    _internal->_workers.push_back(std::unique_ptr<ITreeWorker>(worker));
}
} // namespace world