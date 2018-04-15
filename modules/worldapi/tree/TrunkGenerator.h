#pragma once

#include "core/WorldConfig.h"

#include "math/Bezier.h"
#include "ITreeWorker.h"
#include "TreeSkeletton.h"
#include "Tree.h"

namespace world {

#define DEFAULT_SEGMENT_COUNT 12
#define DEFAULT_MERGE_SIZE 0.04
#define DEFAULT_FACE_SIZE 0.1
#define DEFAULT_MERGE_RES 6

class WORLDAPI_EXPORT TrunkGenerator : public ITreeWorker {
public:
    /** @param segmentCount Nombre de segments.
@param faceSize Taille d'une face sur la longueur d'une branche, en moyenne.
@param mergeSize Longueur de la zone de fusion entre deux branches.
@param mergeRes nombre de cercles dans une zone de fusion entre deux branches.*/
    TrunkGenerator(int segmentCount = DEFAULT_SEGMENT_COUNT,
                   double faceSize = DEFAULT_FACE_SIZE,
                   double mergeSize = DEFAULT_MERGE_SIZE,
                   int mergeRes = DEFAULT_MERGE_RES);

    ~TrunkGenerator();

    TrunkGenerator *clone() const override;

    void process(Tree &tree) override;

private:
    void fillBezier(Mesh &trunkMesh, const BezierCurve &bezier,
                    int divisionCount, double startWeight, double endWeight,
                    int mergePos) const;

    void populateTrunkMesh(Mesh &trunkMesh, const Node<TreeInfo> *node,
                           double mergeLen, double mergeWeight) const;

    int _segmentCount;
    double _faceSize;
    double _mergeSize;
    int _mergeRes;
};
}
