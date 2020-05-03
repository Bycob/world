#include <world/assets/ImageUtils.h>
#include "MultilayerGroundTexture.h"

namespace world {

WORLD_REGISTER_BASE_CLASS(ITextureProvider)

WORLD_REGISTER_CHILD_CLASS(ITerrainWorker, MultilayerGroundTexture,
                           "MultilayerGroundTexture")

using MultilayerElement = MultilayerGroundTexture::Element;

MultilayerGroundTexture::MultilayerGroundTexture() = default;

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

GridStorageBase *MultilayerGroundTexture::getStorage() { return &_storage; }

void MultilayerGroundTexture::write(WorldFile &wf) const {
    wf.addArray("layers");

    for (auto &layer : _layers) {
        wf.addToArray("layers", world::serialize(layer));
    }

    if (_texProvider) {
        wf.addChild("texProvider", _texProvider->serializeSubclass());
    }
}

void MultilayerGroundTexture::read(const WorldFile &wf) {
    for (auto it = wf.readArray("layers"); !it.end(); ++it) {
        _layers.push_back(world::deserialize<DistributionParams>(*it));
    }

    if (wf.hasChild("texProvider")) {
        _texProvider = std::unique_ptr<ITextureProvider>(
            readSubclass<ITextureProvider>(wf.readChild("texProvider")));
    }
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

    if (_texProvider == nullptr) {
        throw std::runtime_error("Texture provider is nullptr");
    }

    const int imWidth = image.width();
    const int imHeight = image.height();
    const u32 tRes = u32(terrain.getResolution());
    Image proxy(imWidth, imHeight, ImageType::RGBA);

    // Precomputing
    double terrainSize = terrain.getBoundingBox().getDimensions().x;
    const double thresholdFactor = 5.0 * exp(-tc._lod / 4.0);

    if (tc._lod == 0) {
        _texProvider->setBasePixelSize(terrainSize / imWidth);
    }

    // generate perlin matrix
    PerlinInfo pinfo{2 + tc._lod, 0.8, false, tc._lod, 4};
    int factor = 4;
    pinfo.offsetX = tc._pos.x * factor;
    pinfo.offsetY = tc._pos.y * factor;

    // Limiting the number of octaves for performances
    const u32 maxOctaves = 15;
    if (pinfo.octaves > maxOctaves) {
        pinfo.octaves = maxOctaves;
    }
    auto perlinMat = _perlin.generatePerlinNoise2D(tRes, pinfo);

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
                vec2d uv = vec2d{vec2u{x, y}} / (tRes - 1);
                double h = terrain.getExactHeightAt(uv.x, uv.y);
                double dh =
                    std::atan(terrain.getSlopeAt(uv.x, uv.y)) * 2.0 / M_PI;

                double r1 = ramp(params.ha, params.hb, params.hc, params.hd,
                                 params.hmin, params.hmax, h);
                double r2 = ramp(params.dha, params.dhb, params.dhc, params.dhd,
                                 params.dhmin, params.dhmax, dh);
                double r = r1 * r2;
                double t = perlinMat(x, y);

                double threshold = params.threshold * thresholdFactor;
                distrib(x, y) = smoothstep(r + threshold, r - threshold, t);
            }
        }

        // Sum with final image
        Image &layerTex = _texProvider->getTexture(layer, tc._lod);
        const int texWidth = layerTex.width();
        const int texHeight = layerTex.height();
        vec2i offset(world::mod<int>(tc._pos.x * imWidth, texWidth),
                     world::mod<int>(tc._pos.y * imHeight, texHeight));

        for (int y = 0; y < imHeight; ++y) {
            for (int x = 0; x < imWidth; ++x) {
                auto origin = proxy.rgba(x, y);
                auto texPix = layerTex.rgba((x + offset.x) % texWidth,
                                            (y + offset.y) % texHeight);

                vec2d uv =
                    vec2d{vec2i{x, y}} / vec2i{imWidth - 1, imHeight - 1};
                double p = distrib.getCubicHeight(uv.x, uv.y);
                double alpha = clamp(p * texPix.getAlphaf(), 0, 1);

                auto &dest = proxy.rgba(x, y);
                dest.setf(
                    origin.getRedf() * (1 - alpha) + texPix.getRedf() * alpha,
                    origin.getGreenf() * (1 - alpha) +
                        texPix.getGreenf() * alpha,
                    origin.getBluef() * (1 - alpha) + texPix.getBluef() * alpha,
                    1);
            }
        }
    }

    image = ImageUtils::toType(proxy, ImageType::RGB);
}
} // namespace world
