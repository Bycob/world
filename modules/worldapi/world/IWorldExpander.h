#pragma once

#include <worldapi/worldapidef.h>

#include "IPointOfView.h"

class World;

class WORLDAPI_EXPORT IWorldExpander {
public:
	virtual ~IWorldExpander() {}

	virtual void expand(World & world, const IPointOfView & from) = 0;
};

