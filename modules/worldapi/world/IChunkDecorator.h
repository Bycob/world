#pragma once

#include <worldapi/worldapidef.h>

class Chunk;

template <typename T> 
class IChunkDecorator {
public:
	virtual ~IChunkDecorator() = default;

	virtual void decorate(T & world, Chunk & chunk) = 0;
};