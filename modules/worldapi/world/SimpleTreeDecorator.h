#pragma once

#include <worldapi/worldapidef.h>

#include <random>

#include "FlatWorld.h"
#include "../tree/TreeGenerator.h"
#include "../tree/TreeSkelettonGenerator.h"

class WORLDAPI_EXPORT SimpleTreeDecorator : IFlatWorldChunkDecorator {
public:
	SimpleTreeDecorator(int maxTreesPerChunk = 40);
	virtual ~SimpleTreeDecorator();

	void setTreeSkelettonGenerator(TreeSkelettonGenerator * generator);
	void setTreeGenerator(TreeGenerator * generator);

	virtual void decorate(FlatWorld & world, Chunk & chunk);

private:
	int _maxTreesPerChunk;

	std::mt19937 _rng;

	std::unique_ptr<TreeSkelettonGenerator> _skelettonGenerator;
	std::unique_ptr<TreeGenerator> _treeGenerator;
};

