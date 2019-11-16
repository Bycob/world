#include "AltitudeTexturer.h"

namespace world {

AltitudeTexturer::AltitudeTexturer()
        : _rng(static_cast<u32>(time(NULL))), _colorMap({513, 65}) {}

ColorMap &AltitudeTexturer::getColorMap() { return _colorMap; }

void AltitudeTexturer::processTerrain(Terrain &terrain) {
    Image &texture = terrain.getTexture();
    auto dims = terrain.getBoundingBox().getDimensions();
    double heightEdgeRatio = dims.z / dims.x;

    std::uniform_real_distribution<double> positive(0, 1);
    std::uniform_real_distribution<double> jitter(-1, 1);

    for (int x = 0; x < texture.width(); x++) {
        for (int y = 0; y < texture.height(); y++) {
            double xd = (double)x / (texture.width() - 1);
            double yd = (double)y / (texture.height() - 1);

            double altitude = terrain.getExactHeightAt(xd, yd);
            double slope = terrain.getSlopeAt(xd, yd);

            // get the parameters to pick in the colormap
            double p1 = clamp(altitude + jitter(_rng) * 0.01, 0, 1);
            double p2 = clamp(atan(abs(slope) * heightEdgeRatio) * 2 / M_PI +
                                  jitter(_rng) * 0.01,
                              0, 1);

            // pick the color
            Color4d color = _colorMap.getColorAt({p1, p2});

            // jitter the color and set in the texture
            double j = 5. / 255.;
            texture.rgb(x, y).setf(clamp(color._r + jitter(_rng) * j, 0, 1),
                                   clamp(color._g + jitter(_rng) * j, 0, 1),
                                   clamp(color._b + jitter(_rng) * j, 0, 1));
        }
    }
}

void AltitudeTexturer::processTile(ITileContext &context) {
    processTerrain(context.getTile().terrain());
}
} // namespace world
