#pragma once

#include <worldapi/worldapidef.h>

class Chunk;

template <typename T> 
class IWorldDecorator {
public:
	virtual ~IWorldDecorator() = default;

	virtual void decorate(T & world, WorldZone &zone) = 0;
};