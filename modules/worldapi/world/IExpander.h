#pragma once

#include <worldapi/worldapidef.h>

#include "IPointOfView.h"

template <typename T> 
class IExpander {
public:
	virtual ~IExpander() = default;

	virtual void expand(T & object, const IPointOfView & from) = 0;
};

