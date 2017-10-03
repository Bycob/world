#pragma once
#include <worldapi/worldapidef.h>

#include "World.h"
#include "WorldGenerator.h"
#include "Environment2D.h"
#include "IExpander.h"
#include "IChunkDecorator.h"

class FlatWorld;

typedef IExpander<FlatWorld> IFlatWorldExpander;
typedef IChunkDecorator<FlatWorld> IFlatWorldChunkDecorator;

class PrivateFlatWorld;

class WORLDAPI_EXPORT FlatWorld : public World {
public:
	FlatWorld();
	virtual ~FlatWorld();

	void addFlatWorldExpander(IFlatWorldExpander * expander);
	void addFlatWorldChunkDecorator(IFlatWorldChunkDecorator * decorator);

	Environment2D & environment() { return _environment; }
	const Environment2D & getEnvironment() const { return _environment; }

protected:
	virtual void callExpanders(const IPointOfView & from);

	virtual void generateChunk(Chunk & chunk);

private:
	PrivateFlatWorld * _internal;

	Environment2D _environment;
};

