#pragma once

#include <worldapi/worldapidef.h>

#include <memory>

template <class T> class GenBase {
public:
	GenBase();
	virtual ~GenBase();

	virtual std::unique_ptr<T> generate() const = 0;

protected :
};

#include "GenBase.inl"

