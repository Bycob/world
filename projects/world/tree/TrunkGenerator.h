#pragma once

#include "world/core/WorldConfig.h"

#include "world/math/Bezier.h"
#include "ITreeWorker.h"
#include "TreeSkeletton.h"
#include "Tree.h"

namespace world {

class WORLDAPI_EXPORT TrunkGenerator : public ITreeWorker {
    WORLD_WRITE_SUBCLASS_METHOD
public:
    TrunkGenerator(int segmentCount = 12);

    ~TrunkGenerator();

    TrunkGenerator *clone() const override;

    void processInstance(TreeInstance &tree, double resolution) override;

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    int _segmentCount;

    void addNode(Mesh &mesh, SkelettonNode<TreeInfo> *node,
                 const vec3d &direction, double resolution, int joinId,
                 bool writeVertIds) const;

    /// Add a tube around a bezier curve, with a smoothed radius between start
    /// and end. `joinId` is the starting id of the ring that starts the tube.
    void addBezierTube(Mesh &mesh, const BezierCurve &curve, int cutCount,
                       double startRadius, double endRadius, int joinId) const;

    /// Add a ring of vertices centered on origin and on the plane defined
    /// by xvec and yvec.
    void addRing(Mesh &mesh, const vec3d &origin, const vec3d &xvec,
                 const vec3d &yvec, double radius) const;

    /// v = the uv's v. Usually i / _segmentCount
    void setLastRingUV(Mesh &mesh, double v) const;

    /// `start1` is the starting id of the lower ring, `start2` is the starting
    /// id of the upper ring
    void addFaces(Mesh &mesh, int start1, int start2) const;
};
} // namespace world
