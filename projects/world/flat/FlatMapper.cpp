#include "FlatMapper.h"

namespace world {

FlatMapper::FlatMapper() : _terrainColors({1, 256}), _zbounds(-2000, 4000) {
    setupDefaultColorScheme();
    _terrainColors.createImage()->write("terrainColors.png");
}

void FlatMapper::map(FlatWorld &world, Image &output,
                     const BoundingBox &bounds) {
    // terrain
    if (_terrain) {
        auto &ground = world.ground();
        const auto &dims = bounds.getDimensions();
        const auto &lower = bounds.getLowerBound();
        const auto &upper = bounds.getUpperBound();
        const int w = output.width(), h = output.height();
        const double res = max(w / dims.x, h / dims.y);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                double xr = lower.x + (x / double(w - 1)) * dims.x;
                double yr = lower.y + (y / double(h - 1)) * dims.y;
                double altitude =
                    clamp((ground.observeAltitudeAt(
                               xr, yr, res, ExplorationContext::getDefault()) -
                           _zbounds.x) /
                              (_zbounds.y - _zbounds.x),
                          0, 1);
                output.rgb(x, y) = _terrainColors.getColorAt({0.5, altitude});
            }
        }
    }
}

ColorMap &FlatMapper::terrainColors() { return _terrainColors; }

void FlatMapper::setZBounds(double min, double max) {
    _zbounds.x = min;
    _zbounds.y = max;
}

void FlatMapper::setupDefaultColorScheme() {
    _terrainColors.addPoint({0.5, 0}, {0.21, 0.25, 0.52});
    _terrainColors.addPoint({0.5, 0.32}, {0.33, 0.37, 0.69});
    _terrainColors.addPoint({0.5, 0.33}, {0.56, 0.58, 0.42});
    _terrainColors.addPoint({0.5, 0.42}, {0.56, 0.87, 0.58});
    _terrainColors.addPoint({0.5, 0.66}, {0.33, 0.52, 0.35});
    _terrainColors.addPoint({0.5, 0.67}, {0.46, 0.46, 0.46});
    _terrainColors.addPoint({0.5, 0.88}, {0.35, 0.35, 0.35});
    _terrainColors.addPoint({0.5, 0.89}, {0.88, 0.88, 0.88});
    _terrainColors.addPoint({0.5, 1}, {1, 1, 1});
    _terrainColors.setOrder(2);
}

} // namespace world