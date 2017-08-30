#pragma once
#include <worldapi/worldapidef.h>

#include "World.h"
#include "WorldGenerator.h"
#include "Environment2D.h"
#include "IFlatWorldExpander.h"

class PrivateFlatWorld;

class WORLDAPI_EXPORT FlatWorld : public World {
public:
	FlatWorld();
	virtual ~FlatWorld();

	void addFlatWorldExpander(IFlatWorldExpander * expander);

	virtual void expand(const IPointOfView & from);

	Environment2D & getEnvironment() { return _environment; }
	const Environment2D & getEnvironment() const { return _environment; }
private:
	PrivateFlatWorld * _internal;

	Environment2D _environment;
};

