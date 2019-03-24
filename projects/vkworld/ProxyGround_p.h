#ifndef VKWORLD_PPROXYGROUND
#define VKWORLD_PPROXYGROUND

#include <map>
#include <memory>

#include <world/core/TileSystem.h>

#include "VkSubBuffer.h"
#include "VkWorker.h"

namespace world {

class ProxyGroundDataPrivate {
public:
    struct VkData {
        VkSubBuffer _upscaleData;
        VkSubBuffer _height;
        VkWorker _worker;
    };

    std::unique_ptr<VkData> _vkData;
};

class ProxyGroundPrivate {
public:
    u32 _tileCount;
    BoundingBox _bbox;

    TileSystem _tileSystem;

    std::map<TileCoordinates, std::unique_ptr<ProxyGroundDataPrivate>> _data;

    ProxyGroundPrivate(f64 tileWidth, u32 tileResolution, u32 tileCount)
            : _tileCount(tileCount),
              _tileSystem(0,
                          vec3i{static_cast<s32>(tileResolution),
                                static_cast<s32>(tileResolution), 0},
                          vec3d{tileWidth, tileWidth, 0}) {

        f64 hwidth = tileWidth * tileCount * 0.5;
        _bbox.reset({-hwidth, -hwidth, 0}, {hwidth, hwidth, 0});
    }
};

} // namespace world

#endif // VKWORLD_PPROXYGROUND