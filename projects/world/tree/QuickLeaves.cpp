#include "QuickLeaves.h"

#include "Tree.h"
#include "world/assets/MeshOps.h"

namespace world {

WORLD_REGISTER_CHILD_CLASS(ITreeWorker, QuickLeaves, "QuickLeaves")

QuickLeaves::QuickLeaves() {}

void QuickLeaves::processInstance(TreeInstance &instance, double resolution) {
    auto &simpleLeaves = instance.leavesMesh(resolution);

    // TODO set better boundaries:
    // - minimum resolution of the instance
    // - resolution at which the leaves are no longer "quick"
    // (defined by parameters, and the leavesgenerator adapts to it?)
    if (resolution < 2 || resolution >= 7 || !simpleLeaves.empty())
        return;

    // TODO utiliser l'algorithme "QuickHull" pour générer des enveloppes
    // convexes
    vec3d leavesCenter{0, 0, 1};
    float radius[] = {0.8f, 1.5f, 1.2f, 0.4f};
    float height[] = {-0.35f, -0.05f, 0.5f, 0.9f};

    const int ringCount = 4;
    const int segmentCount = 7;
    const double uvRatio = 4;

    for (int i = 0; i < ringCount; ++i) {
        for (int j = 0; j < segmentCount; ++j) {
            double angle = M_PI * 2 * j / segmentCount;
            vec3d vert =
                leavesCenter + vec3d{cos(angle) * radius[i],
                                     sin(angle) * radius[i], height[i]};
            vec3d norm = (vert - leavesCenter).normalize();
            vec2d uv{(double(i) / (ringCount - 1)) * uvRatio,
                     (double(j) / (segmentCount - 1)) * uvRatio};
            simpleLeaves.newVertex(vert, norm, uv);

            if (i != ringCount - 1) {
                int ringOffset = segmentCount * i;
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

    MeshOps::recalculateNormals(simpleLeaves);
}

void QuickLeaves::processTree(Tree &tree, double resolution) {
    // TODO create low res texture of leaves
}

QuickLeaves *QuickLeaves::clone() const { return new QuickLeaves(); }

void QuickLeaves::write(WorldFile &wf) const {}

void QuickLeaves::read(const WorldFile &wf) {}

} // namespace world