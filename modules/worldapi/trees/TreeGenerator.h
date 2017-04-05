#pragma once

#include <worldapi/worldapidef.h>

#include "../GenBase.h"
#include "../maths/Bezier.h"

#include "TreeSkeletton.h"
#include "Tree.h"

#define DEFAULT_SEGMENT_COUNT 48
#define DEFAULT_MERGE_SIZE 0.04
#define DEFAULT_FACE_SIZE 0.1
#define DEFAULT_MERGE_RES 8

using maths::BezierCurve;

class WORLDAPI_EXPORT TreeGenerator : public GenBase<Tree> {
public:
	TreeGenerator();
	~TreeGenerator();

	std::unique_ptr<Tree> generate(const TreeSkeletton & skeletton) const;

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

