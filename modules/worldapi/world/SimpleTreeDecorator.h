#pragma once

#include <worldapi/worldapidef.h>

#include <random>

#include "FlatWorld.h"
#include "FlatWorldGenerator.h"
#include "../tree/TreeGenerator.h"
#include "../tree/TreeSkelettonGenerator.h"

class WORLDAPI_EXPORT SimpleTreeDecorator : public IFlatWorldChunkDecorator {
public:
	SimpleTreeDecorator(int maxTreesPerChunk = 1);
	SimpleTreeDecorator(const SimpleTreeDecorator & other);
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

