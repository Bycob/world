#pragma once

#include "core/WorldConfig.h"

#include "WorldZone.h"

namespace world {

template <typename T> class IWorldDecorator {
public:
    virtual ~IWorldDecorator() = default;

    virtual void decorate(T &world, const WorldZone &zone) = 0;
};
} // namespace world