#include <world/assets/ImageUtils.h>
#include "MultilayerGroundTexture.h"

namespace world {

WORLD_REGISTER_BASE_CLASS(ITextureProvider)

WORLD_REGISTER_CHILD_CLASS(ITerrainWorker, MultilayerGroundTexture,
                           "MultilayerGroundTexture")

using MultilayerElement = MultilayerGroundTexture::Element;

MultilayerGroundTexture::MultilayerGroundTexture() = default;

void MultilayerGroundTexture::processTerrain(Terrain &terrain) {
    process(terrain, terrain.getTexture(), {},
            ExplorationContext::getDefault());
}

void MultilayerGroundTexture::processTile(ITileContext &context) {
    process(context.getTile().terrain(), context.getTile().texture(),
            context.getCoords(), context.getExplorationContext());
}

void MultilayerGroundTexture::collectTile(ICollector &collector,
                                          ITileContext &context) {
    if (collector.hasChannel<Image>()) {
        auto &imgChannel = collector.getChannel<Image>();
        const ExplorationContext &ctx = context.getExplorationContext();
        TileCoordinates tc = context.getCoords();

        // Textures (if not collected already)
        collectTextures(imgChannel, tc, ctx);

        // Distribution
        ItemKey baseKey = context.getCoords().toKey();
        auto &tileElem = _storage.get(context.getCoords());

        for (size_t i = 0; i < tileElem._distributions.size(); ++i) {
            ItemKey key = getDistributionKey(tc, i);
            // TODO create image at each distribution is bad for performances
            // TODO reduce distribution size in comparison to texture size (this
            // will be required for large terrains)
            imgChannel.put(key, tileElem._distributions.at(i).createImage(),
                           ctx);
        }
    }
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
                                      const TileCoordinates &tc,
                                      const ExplorationContext &ctx) {

    if (_texProvider == nullptr) {
        throw std::runtime_error("Texture provider is nullptr");
    }

    const int imWidth = image.width();
    const int imHeight = image.height();
    const u32 dRes = _distribResolution;
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
    auto perlinMat = _perlin.generatePerlinNoise2D(dRes, pinfo);

    MultilayerElement &elem = _storage.getOrCreate(tc);

    for (size_t layer = 0; layer < _layers.size(); ++layer) {
        // Compute distribution
        elem._distributions.emplace_back(dRes);
        Terrain &distrib = elem._distributions.back();

        DistributionParams &params = _layers.at(layer);

        for (u32 y = 0; y < dRes; ++y) {
            for (u32 x = 0; x < dRes; ++x) {
                // See shader distribution-height.frag in vkworld for more
                // details
                vec2d uv = vec2d{vec2u{x, y}} / (dRes - 1);
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
        // TODO allow to disable this part
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

    // Setup terrain material
    Material &terrainMat = terrain.getMaterial();

    for (size_t i = 0; i < _layers.size(); ++i) {
        std::string distribName(ctx(getDistributionKey(tc, i)).str());
        terrainMat.setCustomMap("distribution" + std::to_string(i),
                                distribName);
        std::string texName(ctx(getTextureKey(tc, i)).str());
        terrainMat.setCustomMap("texture" + std::to_string(i), texName);
    }
}

void MultilayerGroundTexture::collectTextures(
    ICollectorChannel<Image> &texChannel, const TileCoordinates &tc,
    const ExplorationContext &ctx) {

    // TODO this may change with biomes update
    int lod = tc._lod;

    for (size_t i = 0; i < _layers.size(); ++i) {
        ItemKey texKey = getTextureKey(tc, i);

        if (!texChannel.has(texKey, ctx)) {
            texChannel.put(texKey, _texProvider->getTexture(i, lod), ctx);
        }
    }
}

ItemKey MultilayerGroundTexture::getDistributionKey(const TileCoordinates &tc,
                                                    int id) const {
    return {tc.toKey(), NodeKeys::fromInt(id)};
}

ItemKey MultilayerGroundTexture::getTextureKey(const TileCoordinates &tc,
                                               int id) const {
    return std::to_string(id) + "_" + std::to_string(tc._lod);
}
} // namespace world
