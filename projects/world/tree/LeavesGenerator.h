#ifndef WORLD_LEAVES_GENERATOR_H
#define WORLD_LEAVES_GENERATOR_H

#include "world/core/WorldConfig.h"

#include <random>

#include "ITreeWorker.h"
#include "TreeSkeletton.h"
#include "world/assets/Mesh.h"

namespace world {

class WORLDAPI_EXPORT LeavesGenerator : public ITreeWorker {
public:
    LeavesGenerator(double leafDensity = 0.2, double weightThreshold = 0.2);

    void setLeafDensity(double density);

    void process(Tree &tree) override;

    LeavesGenerator *clone() const override;

private:
    std::mt19937 _rng;
    std::uniform_real_distribution<double> _distrib;

    double _leafDensity;
    double _weightThreshold;

    void processNode(Node<TreeInfo> &node, Mesh &leavesMesh, Mesh &trunkMesh);

    void addLeaf(Mesh &mesh, const vec3d &position, const vec3d &normal);
};

} // namespace world

#endif // WORLD_LEAVES_GENERATOR_H
