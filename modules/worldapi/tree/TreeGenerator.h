#pragma once

#include <worldapi/worldapidef.h>

#include "../ICloneable.h"
#include "../maths/Bezier.h"
#include "TreeSkeletton.h"
#include "Tree.h"

#define DEFAULT_SEGMENT_COUNT 12
#define DEFAULT_MERGE_SIZE 0.04
#define DEFAULT_FACE_SIZE 0.1
#define DEFAULT_MERGE_RES 6

using maths::BezierCurve;

class WORLDAPI_EXPORT TreeGenerator : public ICloneable<TreeGenerator> {
public:
	TreeGenerator();
	~TreeGenerator();

	TreeGenerator * clone() const override;

	Tree * generate(const TreeSkeletton & skeletton) const;
private :
	void fillBezier(Mesh &trunkMesh, const BezierCurve & bezier, int divisionCount,
		double startWeight, double endWeight, int mergePos) const;
	void populateTrunkMesh(Mesh & trunkMesh, const Node<TreeInfo>* node,
		double mergeLen, double mergeWeight) const;

	int _segmentCount = DEFAULT_SEGMENT_COUNT;
	float _faceSize = DEFAULT_FACE_SIZE;
	float _mergeSize = DEFAULT_MERGE_SIZE;
	int _mergeRes = DEFAULT_MERGE_RES;
};

