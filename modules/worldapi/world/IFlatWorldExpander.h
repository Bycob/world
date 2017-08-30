#pragma once

#include <worldapi/worldapidef.h>

#include "IPointOfView.h"

class FlatWorld;

class WORLDAPI_EXPORT IFlatWorldExpander {
public:
	virtual ~IFlatWorldExpander() {}

	virtual void expand(FlatWorld & world, const IPointOfView & from) = 0;
};

