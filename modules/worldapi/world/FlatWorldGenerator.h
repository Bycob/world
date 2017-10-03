#pragma once
#include <worldapi/worldapidef.h>

#include "WorldGenerator.h"
#include "FlatWorld.h"

class WORLDAPI_EXPORT FlatWorldGenNode : public WorldGenNode {
public:
	virtual ~FlatWorldGenNode() = default;

	void startGeneration(FlatWorld & world);
	
	virtual void generate(FlatWorld & world) = 0;
	virtual void generate(World & world) {}

	void addFlatWorldNode(FlatWorldGenNode * node);
private:
	void generateChildren(FlatWorld & world);

	std::vector<FlatWorldGenNode*> _children;
};

class PrivateFlatWorldGenerator;

class WORLDAPI_EXPORT FlatWorldGenerator : public WorldGenerator {
public:
	FlatWorldGenerator();
	virtual ~FlatWorldGenerator();

	void addPrimaryNode(FlatWorldGenNode * node);

	virtual World * generate();
	FlatWorld * generateFlatWorld();
protected:
	void processFlatWorldGeneration(FlatWorld & world);

private:
	PrivateFlatWorldGenerator * _internal;
};

