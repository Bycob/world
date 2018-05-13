#include "LeavesGenerator.h"

#include "Tree.h"

namespace world {

LeavesGenerator::LeavesGenerator(double leafDensity, double weightThreshold)
        : _rng(time(NULL)), _distrib(0, 1), _leafDensity(leafDensity), _weightThreshold(weightThreshold)  {}

void LeavesGenerator::setLeafDensity(double density) {
    _leafDensity = density;
}

void LeavesGenerator::process(Tree &tree) {
    Mesh &leaves = tree.leavesMesh();
    Mesh &trunk = tree.getTrunkMesh();
    TreeSkeletton &skeletton = tree.getSkeletton();

    processNode(*skeletton.getPrimaryNode(), leaves, trunk);
}

LeavesGenerator* LeavesGenerator::clone() const {
    return new LeavesGenerator(*this);
}

void LeavesGenerator::processNode(Node<TreeInfo> &node, Mesh &leavesMesh, Mesh &trunkMesh) {
    auto &nodeInfo = node.getInfo();

    if (node.getWeight() < _weightThreshold) {
        for (int i = nodeInfo._firstVert; i < nodeInfo._lastVert; ++i) {
            if (_leafDensity > _distrib(_rng)) {
                auto &vert = trunkMesh.getVertex(i);
                addLeaf(leavesMesh, vert.getPosition(), vert.getNormal());
            }
        }
    }

    auto &children = node.getChildrenOrNeighboursAccess();

    for (auto child : children) {
        processNode(*child, leavesMesh, trunkMesh);
    }
}

void LeavesGenerator::addLeaf(Mesh &mesh, const vec3d &position, const vec3d &normal) {
    // Compute missing base vectors
    vec3d ez{0, 0, 1};
    vec3d ax = ez.crossProduct(normal);

    if (ax.norm() < std::numeric_limits<double>::epsilon()) {
        ax = {1, 0, 0};
    }
    else {
        ax = ax.normalize();
    }

    vec3d ay = ax.crossProduct(normal);

    // Create a square
    double hwidth = 0.04;
    double height = 0.13;
    double angle = _distrib(_rng) * M_PI;

    vec3d sideVec = ax * cos(angle) + ay * sin(angle);

    vec3d v1 = position + sideVec * hwidth;
    vec3d v2 = position - sideVec * hwidth;

    int idStart = mesh.getVerticesCount();
    mesh.newVertex(v1);
    mesh.newVertex(v2);
    mesh.newVertex(v2 + normal * height);
    mesh.newVertex(v1 + normal * height);

    int ids[][3] = {
        {idStart, idStart + 1, idStart + 2},
        {idStart, idStart + 2, idStart + 3}
    };
    mesh.newFace(ids[0]);
    mesh.newFace(ids[1]);
}

} // namespace world
