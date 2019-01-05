#ifndef VKWORLD_GROUNDDECORATOR_H
#define VKWORLD_GROUNDDECORATOR_H

#include "VkWorldConfig.h"

#include <world/core/IWorldDecorator.h>
#include <world/flat/FlatWorld.h>

namespace world {

class VKWORLD_EXPORT GroundDecorator : public FlatWorldDecorator {
public:
    GroundDecorator();
    ~GroundDecorator() override;

    void decorate(FlatWorld &world, const WorldZone &zone) override;
};
} // namespace world

#endif // VKWORLD_GROUNDDECORATOR_H