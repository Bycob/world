#include "Tree.h"

#include <vector>

#include "world/core/IResolutionModel.h"
#include "world/assets/SceneNode.h"
#include "world/assets/MeshOps.h"
#include "TreeSkelettonGenerator.h"
#include "TrunkGenerator.h"
#include "LeavesGenerator.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(WorldNode, Tree, "Tree");
WORLD_SECOND_REGISTER_CHILD_CLASS(IInstanceGenerator, Tree, "Tree")


const double SIMPLE_RES = 2;
const double BASE_RES = 7;

TreeInstance::TreeInstance(vec3d pos) : _pos(pos) {
    auto &simpleLod = addLod(SIMPLE_RES, 2);

    Material &simpleTrunkMat = simpleLod.addMaterial();
    simpleLod.getNode(0).setMaterial(simpleTrunkMat);
    simpleLod.getNode(1).setMaterialID("leaves");

    auto &baseLod = addLod(BASE_RES, 2);

    Material &trunkMat = baseLod.addMaterial();
    // trunkMat.setKd(0.5, 0.2, 0);
    trunkMat.setMapKd(baseLod.getId(0));

    baseLod.getNode(0).setMaterial(trunkMat);
    baseLod.getNode(1).setMaterialID("leaves");

    baseLod.addTexture();
}

Mesh &TreeInstance::simpleTrunk() { return getLod(0).getMesh(0); }

Mesh &TreeInstance::simpleLeaves() { return getLod(0).getMesh(1); }

Mesh &TreeInstance::trunkMesh() { return getLod(1).getMesh(0); }

Mesh &TreeInstance::leavesMesh() { return getLod(1).getMesh(1); }

Material &TreeInstance::trunkMaterial() { return getLod(1).getMaterial(0); }

Image &TreeInstance::trunkTexture() { return getLod(1).getTexture(0); }

void TreeInstance::reset() {
    _generated = false;

    simpleTrunk() = Mesh();
    simpleLeaves() = Mesh();
    trunkMesh() = Mesh();
    leavesMesh() = Mesh();
}


class PTree {
public:
    std::vector<std::unique_ptr<ITreeWorker>> _workers;

    std::vector<std::unique_ptr<TreeInstance>> _instances;
    BoundingBox _bbox;

    // Common assets
    Image _leavesTexture;

    PTree() : _leavesTexture(1, 1, ImageType::RGBA) {}
};

Tree::Tree() : _internal(new PTree()) {}

Tree::~Tree() { delete _internal; }

void Tree::addTree(vec3d pos) {
    _internal->_instances.emplace_back(std::make_unique<TreeInstance>(pos));

    if (_internal->_instances.size() == 1) {
        _internal->_bbox.reset(pos);
    } else {
        _internal->_bbox.addPoint(pos);
    }
}

Image &Tree::getLeavesTexture() { return _internal->_leavesTexture; }

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

    for (auto &ti : _internal->_instances) {
        templates.push_back(collectTree(*ti, collector, ctx, maxRes));
    }

    return templates;
}

HabitatFeatures Tree::randomize() {
    _internal->_workers.clear();
    reset();

    auto &skeletton = addWorker<TreeSkelettonGenerator>();
    skeletton.setRootWeight(TreeParamsd::gaussian(3, 0.2));
    skeletton.setForkingCount(
        TreeParamsi::WeightThreshold(0.15, TreeParamsi::uniform_int(3, 4)));
    skeletton.setInclination(
        TreeParamsd::PhiOffset(TreeParamsd::gaussian(0.2 * M_PI, 0.05 * M_PI)));
    skeletton.setTheta(
        TreeParamsd::UniformTheta(TreeParamsd::gaussian(0, 0.05 * M_PI)));
    skeletton.setSize(
        TreeParamsd::SizeFactor(TreeParamsd::uniform_real(0.5, 0.75)));

    addWorker<TrunkGenerator>(12);
    addWorker<LeavesGenerator>(0.2, 0.15);

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

    if (res > SIMPLE_RES && ti.simpleTrunk().getVerticesCount() == 0) {
        generateSimpleMeshes(ti);
    }
    if (res > BASE_RES && !ti._generated) {
        generateBase(ti);
    }

    Template tp = ti.collect(collector, ctx, res);
    tp._position = ti._pos;

    if (collector.hasChannel<Material>()) {
        auto &materialsChannel = collector.getChannel<Material>();

        Material leavesMat("leaves");
        leavesMat.setKd(0.4, 0.9, 0.4);

        materialsChannel.put({"leaves"}, leavesMat, ctx);

        // TODO leaves texture
    }

    return tp;
}

void Tree::generateBase(TreeInstance &instance) {
    for (auto &worker : _internal->_workers) {
        worker->processInstance(instance);
    }
    // u32 total = instance._trunkMesh.getVerticesCount() +
    // instance._leavesMesh.getVerticesCount(); std::cout << total << " "  << u
    // / total << std::endl; std::cout << (u += total * sizeof(Vertex)) <<
    // std::endl;

    instance._generated = true;
}

void Tree::generateSimpleMeshes(TreeInstance &instance) {
    // trunk
    // TODO utiliser le générateur d'arbres pour générer une version low
    // poly du tronc avec peu de branches.
    auto &simpleTrunk = instance.simpleTrunk();
    auto &simpleLeaves = instance.simpleLeaves();

    vec3d trunkBottom{};
    vec3d trunkTop = trunkBottom + vec3d{0, 0.3, 2};
    double trunkRadius = 0.2;

    for (int i = 0; i < 3; ++i) {
        double angle = M_PI * 2 * i / 3;
        vec3d shift{cos(angle) * trunkRadius, sin(angle) * trunkRadius, 0};
        simpleTrunk.newVertex(trunkBottom + shift);
        simpleTrunk.newVertex(trunkTop + shift);
        int ids[][3] = {{2 * i, (2 * i + 2) % 6, 2 * i + 1},
                        {(2 * i + 2) % 6, (2 * i + 2) % 6 + 1, 2 * i + 1}};
        simpleTrunk.newFace(ids[0]);
        simpleTrunk.newFace(ids[1]);
    }

    // leaves
    // TODO utiliser l'algorithme "QuickHull" pour générer des enveloppes
    // convexes
    vec3d leavesCenter = trunkTop;
    float radius[] = {0.8f, 1.5f, 1.2f, 0.4f};
    float height[] = {-0.35f, -0.05f, 0.5f, 0.9f};

    const int ringCount = 4;
    const int segmentCount = 7;

    for (int i = 0; i < ringCount; ++i) {
        for (int j = 0; j < segmentCount; ++j) {
            double angle = M_PI * 2 * j / segmentCount;
            vec3d vert =
                leavesCenter + vec3d{cos(angle) * radius[i],
                                     sin(angle) * radius[i], height[i]};
            simpleLeaves.newVertex(vert);

            if (i != ringCount - 1) {
                int ringOffset = segmentCount * i;
                int totalOffset = ringOffset;
                int ids[][3] = {{ringOffset + j, ringOffset + (j + 1) % 7,
                                 ringOffset + segmentCount + j},
                                {ringOffset + (j + 1) % 7,
                                 ringOffset + segmentCount + (j + 1) % 7,
                                 ringOffset + segmentCount + j}};
                simpleLeaves.newFace(ids[0]);
                simpleLeaves.newFace(ids[1]);
            }
        }
    }
    // u32 total = _simpleTrunk.getVerticesCount() +
    // _simpleLeaves.getVerticesCount(); std::cout << "v " << (v += total *
    // sizeof(Vertex)) << " + " << u << std::endl;

    MeshOps::recalculateNormals(simpleTrunk);
    MeshOps::recalculateNormals(simpleLeaves);
}

void Tree::reset() {
    for (auto &ti : _internal->_instances) {
        ti->reset();
    }
}

void Tree::addWorkerInternal(ITreeWorker *worker) {
    _internal->_workers.push_back(std::unique_ptr<ITreeWorker>(worker));
}
} // namespace world