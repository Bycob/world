#ifndef VKWORLD_PROXYGROUND_H
#define VKWORLD_PROXYGROUND_H

#include "VkWorldConfig.h"

#include <world/core/WorldObject.h>
#include <world/core/TileSystem.h>

namespace world {

class ProxyGroundPrivate;
class ProxyGroundDataPrivate;

class VKWORLD_EXPORT ProxyGround : public WorldObject {
public:
    ProxyGround(f64 width, u32 resolution);
    ~ProxyGround();

    void collect(ICollector &collector,
                 const IResolutionModel &resolutionModel) override;

private:
    ProxyGroundPrivate *_internal;


    /** Get data at the specific coordinates. If not found, created. */
    ProxyGroundDataPrivate &getData(const TileCoordinates &tileCoords);
};
} // namespace world

#endif // VKWORLD_PROXYGROUND_H