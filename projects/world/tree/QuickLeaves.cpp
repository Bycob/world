#include "QuickLeaves.h"

#include "Tree.h"
#include "world/assets/MeshOps.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITreeWorker, QuickLeaves, "QuickLeaves")

QuickLeaves::QuickLeaves(double resolution, const Color4d &color)
        : _rng(std::random_device()()), _maxResolution(resolution),
          _color(color) {}

void QuickLeaves::setColor(const Color4d &color) { _color = color; }

void QuickLeaves::processInstance(TreeInstance &instance, double resolution) {
    if (!instance._tree.isTwoMeshes(resolution))
        return;

    auto &leavesMesh = instance.leavesMesh(resolution);

    if (resolution >= _maxResolution || !leavesMesh.empty())
        return;

    BoundingBox trunkBbox{{0}};
    addNode(instance._skeletton.getPrimaryNode(), trunkBbox);

    vec3d low = trunkBbox.getLowerBound();
    vec3d up = trunkBbox.getUpperBound();

    auto leavesCenter =
        trunkBbox.getLowerBound() + trunkBbox.getDimensions() * 0.5;

    BezierCurve curve{{up.x * 0.5, 0, low.z},
                      {0, 0, up.z},
                      {up.x * 1, 0, (up.z - low.z) * 0.5},
                      {up.x * 1, 0, (low.z - up.z) * 0.2}};

    const int ringCount = 4;
    const int segmentCount = 7;

    for (int i = 0; i < ringCount; ++i) {
        double t = double(i) / ringCount;

        for (int j = 0; j <= segmentCount; ++j) {
            // angle in radians
            double angle = M_PI * 2 * j / segmentCount;
            double sin = std::sin(angle);
            double cos = std::cos(angle);

            vec3d v = curve.getPointAt(t);
            v = {v.x * cos - v.y * sin, v.x * sin + v.y * cos, v.z};
            vec3d n = (v - leavesCenter).normalize();
            vec2d uv{double(i) / (ringCount - 1),
                     double(j) / (segmentCount - 1)};

            leavesMesh.newVertex(v, n, uv);

            // add faces
            if (j != 0 && i != 0) {
                const int oneRing = segmentCount + 1;
                int ringOffset = oneRing * i;
                int ids[][3] = {{ringOffset - oneRing + j - 1,
                                 ringOffset - oneRing + j, ringOffset + j - 1},
                                {ringOffset - oneRing + j, ringOffset + j,
                                 ringOffset + j - 1}};

                leavesMesh.newFace(ids[0]);
                leavesMesh.newFace(ids[1]);
            }
        }
    }

    // Add final faces
    vec3d top = curve.getPointAt(1);
    auto topId = leavesMesh.getVerticesCount();
    leavesMesh.newVertex(top, {0, 0, 1}, {1, 1});

    int ringOffset = (segmentCount + 1) * (ringCount - 1);

    for (int i = 0; i < segmentCount; ++i) {
        leavesMesh.newFace(ringOffset + i, ringOffset + i + 1, topId);
    }

    MeshOps::recalculateNormals(leavesMesh);

    // Texture
    Image &texture = instance.overrideTexture(1, resolution);
    const int size = 128;
    texture = Image(size, size * 2, ImageType::RGBA);
    generateTexture(texture);
}

void QuickLeaves::processTree(Tree &tree, double resolution) {}

QuickLeaves *QuickLeaves::clone() const { return new QuickLeaves(); }

void QuickLeaves::write(WorldFile &wf) const {
    wf.addDouble("resolution", _maxResolution);
    wf.addStruct("color", _color);
}

void QuickLeaves::read(const WorldFile &wf) {
    wf.readDoubleOpt("resolution", _maxResolution);
    wf.readStruct("color", _color);
}

void QuickLeaves::addNode(const TreeNode *node, BoundingBox &bbox, int depth) {
    auto &ti = node->getInfo();
    const int minDepth = 1;

    // Reset bbox if needed
    if (depth == minDepth && bbox.getLowerBound().squaredLength({}) <
                             std::numeric_limits<double>::epsilon()) {
        bbox.reset(ti._position);
    } else if (depth >= minDepth) {
        bbox.addPoint(ti._position);
    }

    // Add all points to the bbox
    for (auto *child : node->getChildrenOrNeighboursList()) {
        addNode(child, bbox, depth + 1);
    }
}

void QuickLeaves::generateTexture(Image &img) {
    int size = img.width();
    std::bernoulli_distribution d(0.25);

    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            img.rgba(x, y).setf(_color._r, _color._g, _color._b, d(_rng));
        }
    }
}

} // namespace world