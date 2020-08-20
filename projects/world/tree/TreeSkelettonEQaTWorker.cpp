#include "TreeSkelettonEQaTWorker.h"

#include <list>

#include "world/math/MathsHelper.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITreeWorker, TreeSkelettonEQaTWorker,
                           "TreeSkelettonEQaTWorker")

TreeSkelettonEQaTWorker::TreeSkelettonEQaTWorker()
        : _rng(Params<double>::uniform_real(0, 1)),

          _seedLocation(TreeParamsd::constant(0)),
          _rootWeight(TreeParamsd::constant(2)),

          _phi(TreeParamsd::PhiOffset(TreeParamsd::constant(M_PI / 6.0))),
          _theta(TreeParamsd::UniformTheta(TreeParamsd::constant(0))),
          _size(TreeParamsd::SizeFactor(TreeParamsd::constant(3.0 / 5.0))),

          _weight(TreeParamsd::DefaultWeight()),
          _count(TreeParamsi::WeightThreshold(0.02, TreeParamsi::constant(4))) {
}

TreeSkelettonEQaTWorker::~TreeSkelettonEQaTWorker() {}

TreeSkelettonEQaTWorker *TreeSkelettonEQaTWorker::clone() const {
    return new TreeSkelettonEQaTWorker(*this);
}

void TreeSkelettonEQaTWorker::setSeedLocation(const TreeParamd &param) {
    _seedLocation = param;
}

void TreeSkelettonEQaTWorker::setRootWeight(const TreeParamd &param) {
    _rootWeight = param;
}

void TreeSkelettonEQaTWorker::setInclination(const TreeParamd &param) {
    _phi = param;
}

void TreeSkelettonEQaTWorker::setTheta(const TreeParamd &param) {
    _theta = param;
}

void TreeSkelettonEQaTWorker::setForkingCount(const TreeParami &param) {
    _count = param;
}

void TreeSkelettonEQaTWorker::setSize(const TreeParamd &size) { _size = size; }

void TreeSkelettonEQaTWorker::setWeight(const TreeParamd &weight) {
    _weight = weight;
}

void TreeSkelettonEQaTWorker::processInstance(TreeInstance &tree,
                                              double resolution) {
    if (!tree._skeletton.empty() || !tree._tree.isTwoMeshes(resolution))
        return;

    SkelettonNode<TreeInfo> *primaryNode = tree._skeletton.getPrimaryNode();
    TreeInfo &info = primaryNode->getInfo();
    info._selfWeight = _rootWeight(TreeInfo(), info);
    info._weight = info._selfWeight;
    const double shapeFactor = 8;
    info._size = pow(info._selfWeight * shapeFactor * shapeFactor, 1. / 3.);
    info._theta = info._phi = 0;
    info._forkCount = 0;
    initBranch(nullptr, primaryNode);

    forkNode(primaryNode);
}

void TreeSkelettonEQaTWorker::randomize() {
    setRootWeight(TreeParamsd::gaussian(3, 0.2));
    setForkingCount(
        TreeParamsi::WeightThreshold(0.15, TreeParamsi::uniform_int(3, 4)));
    setInclination(
        TreeParamsd::PhiOffset(TreeParamsd::gaussian(0.2 * M_PI, 0.05 * M_PI)));
    setTheta(TreeParamsd::UniformTheta(TreeParamsd::gaussian(0, 0.05 * M_PI)));
    setSize(TreeParamsd::SizeFactor(TreeParamsd::uniform_real(0.5, 0.75)));
}

void TreeSkelettonEQaTWorker::write(WorldFile &wf) const {}

void TreeSkelettonEQaTWorker::read(const WorldFile &wf) {}

void TreeSkelettonEQaTWorker::forkNode(SkelettonNode<TreeInfo> *node) {
    TreeInfo &parentInfo = node->getInfo();
    vec3d pos = parentInfo._position;

    // Détermination du nombre de nouvelles branches
    // TODO forkCount in EQaT != forkCount in HoGBased
    parentInfo._forkCount = _count(TreeInfo(), parentInfo);

    // Trick to make the branch connections look nice - part 1
    if (parentInfo._forkCount != 0)
        parentInfo._endArea = 0;

    for (int i = 0; i < parentInfo._forkCount; i++) {
        TreeInfo childInfo;
        childInfo._forkId = i;

        // Détermination du poids de la nouvelle branche
        childInfo._weight = _weight(parentInfo, childInfo);

        // Détermination de l'angle de rotation
        childInfo._theta = _theta(parentInfo, childInfo);

        // Détermination de la nouvelle inclinaison
        childInfo._phi = _phi(parentInfo, childInfo);

        // Détermination de la taille de la nouvelle branche
        childInfo._size = _size(parentInfo, childInfo);

        childInfo._position = {
            pos.x +
                cos(childInfo._theta) * sin(childInfo._phi) * childInfo._size,
            pos.y +
                sin(childInfo._theta) * sin(childInfo._phi) * childInfo._size,
            pos.z + cos(childInfo._phi) * childInfo._size};
        // Ajout du noeud
        SkelettonNode<TreeInfo> *childNode = node->createChild(childInfo);

        initBranch(node, childNode);

        // Trick to make the branch connections look nice - part 2
        double startArea = childNode->getInfo()._startArea;

        if (parentInfo._endArea < startArea) {
            parentInfo._endArea = startArea;
        }

        forkNode(childNode);
    }
}

void TreeSkelettonEQaTWorker::initBranch(SkelettonNode<TreeInfo> *parent,
                                         SkelettonNode<TreeInfo> *child) {
    auto &childInfo = child->getInfo();

    const double bendFactor = 0.5;

    // Set parameters
    childInfo._selfWeight = childInfo._weight;
    childInfo._startArea = childInfo._selfWeight / childInfo._size * 1.1;
    childInfo._endArea =
        2 * childInfo._selfWeight / childInfo._size - childInfo._startArea;

    // Compute the curve
    childInfo._t = 1;

    vec3d ez;
    vec3d origin;

    if (parent != nullptr) {
        auto &parentInfo = parent->getInfo();
        childInfo._forkCount = parentInfo._forkCount + 1;

        const auto parentCurve = parentInfo._curve;
        ez = parentCurve.getDerivativeAt(childInfo._t).normalize();
        origin = parentCurve.getPointAt(childInfo._t);
    } else {
        ez = vec3d::Z();
        origin = {0, 0, _seedLocation(TreeInfo(), childInfo)};
    }

    vec3d ex = vec3d::X();
    vec3d ey = vec3d::Y();

    if (abs(ez.x) + abs(ez.y) >= std::numeric_limits<double>::epsilon() * 2) {
        ey = ez.crossProduct(vec3d::Z()).normalize();
        ex = ey.crossProduct(ez).normalize();
    }

    const double sinPhi = sin(childInfo._phi);
    vec3d local{cos(childInfo._theta) * sinPhi, sin(childInfo._theta) * sinPhi,
                cos(childInfo._phi)};

    // Direction of the branch from start to end.
    vec3d dir = ex * local.x + ey * local.y + ez * local.z;
    vec3d end = origin + dir * childInfo._size;
    vec3d w1 = mix(dir, ez, bendFactor);
    vec3d w2 = mix(-dir, ez - dir * ez.dotProduct(dir) * 2, bendFactor);

    childInfo._curve = {origin, end, w1 * childInfo._size * 0.25,
                        w2 * childInfo._size * 0.25};
    childInfo._position = end;
}
} // namespace world
