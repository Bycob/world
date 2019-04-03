#pragma once

#include "world/core/WorldConfig.h"

#include "world/core/World.h"
#include "IGround.h"

namespace world {
class PFlatWorld;

class WORLDAPI_EXPORT FlatWorld : public World {
public:
    /** Create a complete and rich world that can be used
     * as a demonstration of the API power ! */
    static FlatWorld *createDemoFlatWorld();


    FlatWorld();

    ~FlatWorld() override;

    template <typename T, typename... Args> T &setGround(Args... args);

    IGround &ground();

    void collect(ICollector &collector,
                 const IResolutionModel &resolutionModel) override;

private:
    PFlatWorld *_internal;

    void setGroundInternal(GroundNode *ground);
};

template <typename T, typename... Args> T &FlatWorld::setGround(Args... args) {
    T *ground = new T(args...);
    setGroundInternal(ground);
    return *ground;
}
} // namespace world
