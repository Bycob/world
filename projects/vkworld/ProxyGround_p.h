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
    struct Layer {
        std::unique_ptr<VkwSubBuffer> _repartitionData;
        std::unique_ptr<VkwSubBuffer> _repartition;
        std::unique_ptr<VkwSubBuffer> _textureData;
    };

    std::unique_ptr<VkwSubBuffer> _upscaleData;
    std::unique_ptr<VkwSubBuffer> _height;
    std::unique_ptr<VkwSubBuffer> _texture;

    std::vector<Layer> _layers;

    std::unique_ptr<VkwWorker> _worker;
};

class ProxyGroundPrivate {
public:
    struct LayerInfo {
        std::string _repartitionShader;
        std::string _textureShader;
    };

    u32 _tileCount;
    BoundingBox _bbox;
    std::vector<LayerInfo> _layers;

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