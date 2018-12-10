#include "TrunkGenerator.h"
#include "world/assets/Mesh.h"
#include "world/assets/MeshOps.h"
#include "world/math/MathsHelper.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>

namespace world {

TrunkGenerator::TrunkGenerator(int segmentCount, double resolution)
        : _segmentCount(segmentCount), _resolution(resolution) {}

TrunkGenerator::~TrunkGenerator() {}

TrunkGenerator *TrunkGenerator::clone() const {
    return new TrunkGenerator(*this);
}

double getRadius(double weight) { return pow(weight, 0.75) / 12; }

void TrunkGenerator::process(Tree &tree) {
    // Création du mesh
    Mesh &trunkMesh = tree.getTrunkMesh();
    auto primary = tree.getSkeletton().getPrimaryNode();
    auto &primInfo = primary->getInfo();

    addRing(trunkMesh, primInfo._position, {1, 0, 0}, {0, 1, 0},
            getRadius(primInfo._weight));
    addNode(trunkMesh, primary, {0, 0, 1}, 0, true);
}

void TrunkGenerator::addNode(Mesh &mesh, Node<TreeInfo> *node,
                             const vec3d &direction, int joinId,
                             bool writeVertIds) const {
    auto &nodeInfo = node->getInfo();
    vec3d nodePos = nodeInfo._position;

    auto &children = node->getChildrenOrNeighboursAccess();

    for (auto *child : children) {
        auto &childInfo = child->getInfo();
        vec3d childPos = childInfo._position;
        vec3d newDirection = childPos - nodePos;

        BezierCurve curve(nodePos, childPos, direction * 0.25,
                          newDirection * -0.25);

        if (writeVertIds) {
            childInfo._firstVert = mesh.getVerticesCount();
        }

        addBezierTube(mesh, curve, getRadius(nodeInfo._weight),
                      getRadius(childInfo._weight), joinId);

        if (writeVertIds) {
            childInfo._lastVert = mesh.getVerticesCount();
        }

        addNode(mesh, child, newDirection,
                mesh.getVerticesCount() - _segmentCount, writeVertIds);
    }
}

void TrunkGenerator::addBezierTube(Mesh &mesh, const BezierCurve &curve,
                                   double startRadius, double endRadius,
                                   int joinId) const {

    int cutCount = static_cast<int>(
        ceil(curve._pts[0].length(curve._pts[3]) * _resolution));

    for (int i = 1; i <= cutCount; ++i) {
        double t = static_cast<double>(i) / cutCount;

        vec3d origin = curve.getPointAt(t);
        vec3d direction = curve.getDerivativeAt(t).normalize();

        vec3d ez(0, 0, 1);
        vec3d ax(1, 0, 0);
        vec3d ay(0, 1, 0);

        if (abs(direction.x) + abs(direction.y) >=
            std::numeric_limits<double>::epsilon() * 2) {
            ay = ez.crossProduct(direction).normalize();
            ax = ay.crossProduct(direction).normalize();
        }

        double rf = 1 - exp(-8 * t * t);
        double radius = startRadius * (1 - rf) + endRadius * rf;

        int ringStart = mesh.getVerticesCount();
        addRing(mesh, origin, ax, ay, radius);
        int prevRingId = i == 1 ? joinId : ringStart - _segmentCount;
        addFaces(mesh, prevRingId, ringStart);
    }
}

void TrunkGenerator::addRing(Mesh &mesh, const vec3d &origin, const vec3d &xvec,
                             const vec3d &yvec, double radius) const {

    for (int i = 0; i < _segmentCount; ++i) {
        double angle = i * M_PI * 2. / _segmentCount;
        double cs = cos(angle);
        double sn = sin(angle);

        vec3d dir = xvec * cs + yvec * sn;
        mesh.newVertex(origin + dir * radius, dir);
    }
}

void TrunkGenerator::addFaces(Mesh &mesh, int start1, int start2) const {

    // offset computation
    const int facing = _segmentCount / 2;
    vec3d c1 = mesh.getVertex(start1).getPosition();
    vec3d c2 = mesh.getVertex(start1 + facing).getPosition();
    int offset = 0;
    double min = 1000000;

    for (int i = 0; i < _segmentCount; ++i) {
        vec3d c3 = mesh.getVertex(start2 + i).getPosition();
        vec3d c4 = mesh.getVertex(start2 + ((i + facing) % _segmentCount))
                       .getPosition();
        double sql = c1.squaredLength(c3) + c2.squaredLength(c4);

        if (sql < min) {
            min = sql;
            offset = i;
        }
    }

    // Add faces
    for (int i = 0; i < _segmentCount; ++i) {
        int v1 = i;
        int v2 = (v1 + 1) % _segmentCount;
        int v3 = (offset + i) % _segmentCount;
        int v4 = (v3 + 1) % _segmentCount;

        int ids[] = {start1 + v1, start1 + v2, start2 + v4};
        mesh.newFace(ids);

        ids[1] = start2 + v4;
        ids[2] = start2 + v3;
        mesh.newFace(ids);
    }
}

} // namespace world
