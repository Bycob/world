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

namespace world {

WORLD_REGISTER_CHILD_CLASS(WorldNode, Tree, "Tree");
WORLD_SECOND_REGISTER_CHILD_CLASS(IInstanceGenerator, Tree, "Tree")


const double SIMPLE_RES = 2;
const double BASE_RES = 7;

TreeInstance::TreeInstance(Tree &tree, vec3d pos) : _tree(tree), _pos(pos) {
    auto &simpleLod = addLod(SIMPLE_RES, 2);

    Material &simpleTrunkMat = simpleLod.addMaterial();
    simpleTrunkMat.setKd(0.3, 0.17, 0.13);
    simpleLod.getNode(0).setMaterial(simpleTrunkMat);

    Material &simpleLeafMat = simpleLod.addMaterial();
    simpleLeafMat.setMapKd(simpleLod.getId(0));
    simpleLeafMat.setTransparent(true);
    simpleLod.getNode(1).setMaterial(simpleLeafMat);

    simpleLod.addTexture();

    auto &baseLod = addLod(BASE_RES, 2);

    Material &trunkMat = baseLod.addMaterial();
    // trunkMat.setKd(0.5, 0.2, 0);
    trunkMat.setMapKd(baseLod.getId(0));

    baseLod.getNode(0).setMaterial(trunkMat);
    baseLod.getNode(1).setMaterialID("leaves");

    baseLod.addTexture();
}

Mesh &TreeInstance::trunkMesh(double res) {
    return getLodByResolution(res).getMesh(0);
}

Mesh &TreeInstance::leavesMesh(double res) {
    return getLodByResolution(res).getMesh(1);
}

Material &TreeInstance::trunkMaterial() { return getLod(1).getMaterial(0); }

Image &TreeInstance::trunkTexture() { return getLod(1).getTexture(0); }

void TreeInstance::reset() {
    _resolution = 0;

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            getLod(i).getMesh(j) = Mesh();
        }
    }
}


class PTree {
public:
    std::vector<std::unique_ptr<ITreeWorker>> _workers;

    std::vector<std::unique_ptr<TreeInstance>> _instances;
    BoundingBox _bbox;

    // Common assets
    Image _leavesTexture;
    SpriteGrid _grid;

    /// Resolution at which the instance is currently generated.
    double _resolution = 0;


    PTree() : _leavesTexture(1, 1, ImageType::RGBA), _grid(4) {}
};

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

const SpriteGrid &Tree::getLeavesGrid() { return _internal->_grid; }

TreeInstance &Tree::getTreeInstance(int i) {
    return *_internal->_instances.at(i);
}

void Tree::setup(const Tree &model) {
    _internal->_workers.clear();

    for (auto &worker : model._internal->_workers) {
        addWorkerInternal(worker->clone());
    }

    for (auto &ti : model._internal->_instances) {
        addTree(ti->_pos);
    }
}

void Tree::collect(ICollector &collector,
                   const IResolutionModel &resolutionModel,
                   const ExplorationContext &ctx) {

    // All the trees are too far to be seen
    auto allRes = resolutionModel.getMaxResolutionIn(_internal->_bbox, ctx);
    if (allRes < SIMPLE_RES)
        return;

    if (collector.hasChannel<SceneNode>()) {
        auto &objChan = collector.getChannel<SceneNode>();
        int tpCount = 0;

        for (auto &ti : _internal->_instances) {
            ++tpCount;
            vec3d realPos = ctx.getEnvironment().findNearestFreePoint(
                ti->_pos, {0, 0, 1}, SIMPLE_RES, ctx);
            double resolution = resolutionModel.getResolutionAt(realPos, ctx);

            // Tree is too far to be seen
            if (resolution < SIMPLE_RES)
                continue;

            Template tp = collectTree(*ti, collector, ctx, resolution);
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

        templates.push_back(collectTree(*ti, collector, childCtx, maxRes));

        ++i;
    }

    return templates;
}

HabitatFeatures Tree::randomize() {
    reset();
    _internal->_workers.clear();

    auto &skeletton = addWorker<TreeSkelettonHoGBasedWorker>();
    skeletton.randomize();

    // Trunk and leaves
    addWorker<TrunkGenerator>(12);
    addWorker<LeavesGenerator>(0.2, 0.02);

    // vulkan ?

    while (_internal->_instances.size() < 10) {
        addTree();
    }

    return HabitatFeatures{};
}

void Tree::write(WorldFile &wf) const {
    WorldNode::write(wf);
    wf.addArray("workers");

    for (auto &worker : _internal->_workers) {
        wf.addToArray("workers", worker->serializeSubclass());
    }
}

void Tree::read(const WorldFile &wf) {
    WorldNode::read(wf);

    for (auto it = wf.readArray("workers"); !it.end(); ++it) {
        _internal->_workers.emplace_back(readSubclass<ITreeWorker>(*it));
    }
}

Template Tree::collectTree(TreeInstance &ti, ICollector &collector,
                           const ExplorationContext &ctx, double res) {

    // In reverse order
    if (res > BASE_RES) {
        generate(ti, BASE_RES);
    }
    if (res > SIMPLE_RES) {
        generate(ti, SIMPLE_RES);
    }

    Template tp = ti.collect(collector, ctx, res);
    tp._position = ti._pos;

    // TODO this material may be collected multiple times
    if (collector.hasChannel<Material>()) {
        auto &materialsChannel = collector.getChannel<Material>();

        Material leavesMat("leaves");
        auto &leavesTex = _internal->_leavesTexture;

        if (collector.hasChannel<Image>() &&
            leavesTex.width() * leavesTex.height() != 1) {
            auto &texChannel = collector.getChannel<Image>();

            leavesMat.setKd(1.0, 1.0, 1.0);
            leavesMat.setMapKd(ctx({"leaves"}).str());
            leavesMat.setTransparent(true);
            texChannel.put({"leaves"}, leavesTex, ctx);
        } else {
            // TODO fix this being selected if the trees have not been generated
            // yet (res < 2)
            leavesMat.setKd(0.4, 0.9, 0.4);
        }

        materialsChannel.put({"leaves"}, leavesMat, ctx);
    }

    return tp;
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
    _internal->_leavesTexture = Image(1, 1, ImageType::RGBA);
}

void Tree::addWorkerInternal(ITreeWorker *worker) {
    _internal->_workers.push_back(std::unique_ptr<ITreeWorker>(worker));
}
} // namespace world