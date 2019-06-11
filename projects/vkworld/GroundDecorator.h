#ifndef VKWORLD_GROUNDDECORATOR_H
#define VKWORLD_GROUNDDECORATOR_H

#include "wrappers/VkWorldConfig.h"

#include <world/core/IChunkDecorator.h>
#include <world/flat/FlatWorld.h>

namespace world {

class VKWORLD_EXPORT GroundDecorator : public IChunkDecorator {
public:
    GroundDecorator();
    ~GroundDecorator() override;

    void decorate(Chunk &chunk) override;
};
} // namespace world

#endif // VKWORLD_GROUNDDECORATOR_H