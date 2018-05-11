#pragma once

#include "core/WorldConfig.h"

#include "math/Bezier.h"
#include "ITreeWorker.h"
#include "TreeSkeletton.h"
#include "Tree.h"

namespace world {

class WORLDAPI_EXPORT TrunkGenerator : public ITreeWorker {
public:
    /** @param segmentCount Nombre de segments.
@param faceSize Taille d'une face sur la longueur d'une branche, en moyenne.
@param mergeSize Longueur de la zone de fusion entre deux branches.
@param mergeRes nombre de cercles dans une zone de fusion entre deux branches.*/
    TrunkGenerator(int segmentCount = 12);

    ~TrunkGenerator();

    TrunkGenerator *clone() const override;

    void process(Tree &tree) override;

private:
    int _segmentCount;
    double _resolution;

    void addNode(Mesh &mesh, const Node<TreeInfo> *node, const vec3d &direction, int joinId) const;

    void addBezierTube(Mesh &mesh, const BezierCurve &curve, double startRadius, double endRadius, int joinId) const;

    void addRing(Mesh &mesh, const vec3d &origin, const vec3d &xvec, const vec3d &yvec, double radius) const;

    void addFaces(Mesh &mesh, int start1, int start2) const;
};
}
