#include "MultilayerGroundTexture.h"

namespace world {

using MultilayerElement = MultilayerGroundTexture::Element;

MultilayerGroundTexture::MultilayerGroundTexture(ITextureProvider *texProvider)
        : _texProvider(texProvider) {
    if (_texProvider == nullptr) {
        throw std::runtime_error("Texture provider was initialized to null");
    }
}

void MultilayerGroundTexture::processTerrain(Terrain &terrain) {
    process(terrain, terrain.getTexture(), {});
}

void MultilayerGroundTexture::processTile(ITileContext &context) {
    process(context.getTile().terrain(), context.getTile().texture(),
            context.getCoords());
}

void MultilayerGroundTexture::addLayer(DistributionParams params) {
    _layers.push_back(params);
}

double ramp(double a, double b, double c, double d, double lowb, double highb,
            double x) {
    double ya = (x - a) / (b - a);
    double yc = (d - x) / (d - c);
    double yr = min(ya, yc);
    return clamp(yr, lowb, highb);
}

void MultilayerGroundTexture::process(Terrain &terrain, Image &image,
                                      const TileCoordinates &tc) {

    const u32 imWidth = u32(image.width());
    const u32 imHeight = u32(image.height());
    const u32 tRes = u32(terrain.getResolution());

    MultilayerElement &elem = _storage.getOrCreate(tc);

    for (size_t layer = 0; layer < _layers.size(); ++layer) {
        // Compute distribution
        elem._distributions.emplace_back(tRes);
        Terrain &distrib = elem._distributions.back();

        DistributionParams &params = _layers.at(layer);

        for (u32 y = 0; y < tRes; ++y) {
            for (u32 x = 0; x < tRes; ++x) {
                // See shader distribution-height.frag in vkworld for more
                // details
                vec2d uv = vec2u{x, y} / double(tRes - 1);
                double h = terrain.getExactHeightAt(uv.x, uv.y);
                double dh =
                    std::atan(terrain.getSlopeAt(uv.x, uv.y)) * 2.0 / M_PI;

                double r1 = ramp(params.ha, params.hb, params.hc, params.hd,
                                 params.hmin, params.hmax, h);
                double r2 = ramp(params.dha, params.dhb, params.dhc, params.dhd,
                                 params.dhmin, params.dhmax, dh);
                double r = r1 * r2;
                double t = 0.5; // TODO perlin(noiseParams, uv.x, uv.y, 0);

                distrib(x, y) =
                    smoothstep(r + params.threshold, r - params.threshold, t);
            }
        }

        // Sum with final image
        Image &layerTex = _texProvider->getTexture(layer, tc._lod);
        const u32 texWidth = u32(layerTex.width());
        const u32 texHeight = u32(layerTex.height());
        vec2i offset(world::mod<int>(tc._pos.x * imWidth, texWidth),
                     world::mod<int>(tc._pos.y * imHeight, texHeight));

        for (u32 y = 0; y < imHeight; ++y) {
            for (u32 x = 0; x < imWidth; ++x) {
                auto origin = image.rgba(x, y);
                auto texPix = layerTex.rgba((x + offset.x) % texWidth,
                                            (y + offset.y) % texHeight);

                vec2d uv =
                    vec2u{x, y} / vec2d{vec2u{imWidth - 1, imHeight - 1}};
                double p = distrib.getCubicHeight(uv.x, uv.y);
                double alpha = p * texPix.getAlphaf();

                auto &dest = image.rgba(x, y);
                dest.setf(
                    origin.getRedf() * (1 - alpha) + texPix.getRedf() * alpha,
                    origin.getGreenf() * (1 - alpha) +
                        texPix.getGreenf() * alpha,
                    origin.getBluef() * (1 - alpha) + texPix.getBluef() * alpha,
                    origin.getAlphaf() + (1 - origin.getAlphaf()) * alpha);
            }
        }
    }
}
} // namespace world
