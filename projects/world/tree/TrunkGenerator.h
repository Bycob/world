#pragma once

#include "world/core/WorldConfig.h"

#include "world/math/Bezier.h"
#include "ITreeWorker.h"
#include "TreeSkeletton.h"
#include "Tree.h"

namespace world {

class WORLDAPI_EXPORT TrunkGenerator : public ITreeWorker {
public:
    TrunkGenerator(int segmentCount = 12, double resolution = 20);

    ~TrunkGenerator();

    TrunkGenerator *clone() const override;

    void process(Tree &tree) override;

private:
    int _segmentCount;
    double _resolution;

    void addNode(Mesh &mesh, SkelettonNode<TreeInfo> *node, const vec3d &direction,
                 int joinId, bool writeVertIds) const;

    void addBezierTube(Mesh &mesh, const BezierCurve &curve, double startRadius,
                       double endRadius, int joinId) const;

    void addRing(Mesh &mesh, const vec3d &origin, const vec3d &xvec,
                 const vec3d &yvec, double radius) const;

    void addFaces(Mesh &mesh, int start1, int start2) const;
};
} // namespace world
