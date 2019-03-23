#include "TreeSkelettonGenerator.h"

#include <list>

#include "world/math/MathsHelper.h"

namespace world {

TreeSkelettonGenerator::TreeSkelettonGenerator()
        : _rng(Params<double>::uniform_real(0, 1)),

          _seedLocation(TreeParamsd::constant(0)),
          _rootWeight(TreeParamsd::constant(2)),

          _phi(TreeParamsd::PhiOffset(TreeParamsd::constant(M_PI / 6.0))),
          _theta(TreeParamsd::UniformTheta(TreeParamsd::constant(0))),
          _size(TreeParamsd::SizeFactor(TreeParamsd::constant(3.0 / 5.0))),

          _weight(TreeParamsd::DefaultWeight()),
          _count(TreeParamsi::WeightThreshold(0.02, TreeParamsi::constant(4))) {
}

TreeSkelettonGenerator::~TreeSkelettonGenerator() {}

TreeSkelettonGenerator *TreeSkelettonGenerator::clone() const {
    return new TreeSkelettonGenerator(*this);
}

void TreeSkelettonGenerator::setSeedLocation(const TreeParamd &param) {
    _seedLocation = param;
}

void TreeSkelettonGenerator::setRootWeight(const TreeParamd &param) {
    _rootWeight = param;
}

void TreeSkelettonGenerator::setInclination(const TreeParamd &param) {
    _phi = param;
}

void TreeSkelettonGenerator::setTheta(const TreeParamd &param) {
    _theta = param;
}

void TreeSkelettonGenerator::setForkingCount(const TreeParami &param) {
    _count = param;
}

void TreeSkelettonGenerator::setSize(const TreeParamd &size) { _size = size; }

void TreeSkelettonGenerator::setWeight(const TreeParamd &weight) {
    _weight = weight;
}

void TreeSkelettonGenerator::process(Tree &tree) {
    Node<TreeInfo> *primaryNode = tree.getSkeletton().getPrimaryNode();
    TreeInfo &info = primaryNode->getInfo();
    info._weight = _rootWeight(TreeInfo(), info);
    info._size = info._weight;
    info._position = {0, 0, _seedLocation(TreeInfo(), info)};
    info._forkCount = 1;

    // Add trunk
    TreeInfo secondInfo;
    secondInfo._weight = _weight(info, secondInfo) * 0.5;
    secondInfo._size = _size(info, secondInfo);
    secondInfo._theta = secondInfo._phi = 0;
    secondInfo._position = info._position + vec3d{0, 0, secondInfo._size};

    forkNode(primaryNode->createChild(secondInfo));
}

void TreeSkelettonGenerator::forkNode(Node<TreeInfo> *node) {
    TreeInfo &parentInfo = node->getInfo();
    vec3d pos = parentInfo._position;

    // Détermination du nombre de nouvelles branches
    parentInfo._forkCount = _count(TreeInfo(), parentInfo);

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
        Node<TreeInfo> *childNode = node->createChild(childInfo);

        forkNode(childNode);
    }
}
} // namespace world
