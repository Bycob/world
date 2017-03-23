#pragma once

#include <memory>
#include "worldapidef.h"

template <class T> class GenBase {
public:
	GenBase();
	virtual ~GenBase();

	virtual std::unique_ptr<T> generate() const = 0;

protected :
};

#include "GenBase.inl"

