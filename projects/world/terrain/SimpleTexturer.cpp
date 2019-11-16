#include "SimpleTexturer.h"

namespace world {

SimpleTexturer::SimpleTexturer()
        : _rng(static_cast<u32>(time(NULL))), _colorMap({513, 65}) {}

ColorMap &SimpleTexturer::getColorMap() { return _colorMap; }

void SimpleTexturer::processTerrain(Terrain &terrain) {
    Image &texture = terrain.getTexture();
    auto dims = terrain.getBoundingBox().getDimensions();

    std::uniform_real_distribution<double> positive(0, 1);

    for (int x = 0; x < texture.width(); x++) {
        for (int y = 0; y < texture.height(); y++) {
            double xd = (double)x / (texture.width() - 1);
            double yd = (double)y / (texture.height() - 1);

            double altitude = terrain.getRawHeight(xd, yd);

            // get the parameters to pick in the colormap
            double p1 = clamp(altitude, 0, 1);
            double p2 = positive(_rng);

            // pick the color
            Color4d color = _colorMap.getColorAt({p1, p2});

            // jitter the color and set in the texture
            texture.rgb(x, y).setf(color._r, color._g, color._b);
        }
    }
}

void SimpleTexturer::processTile(ITileContext &context) {
    processTerrain(context.getTile().terrain());
}
} // namespace world
