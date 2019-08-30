#include "Grass.h"

#include "world/assets/SceneNode.h"
#include "../assets/Material.h"
#include "../math/Bezier.h"
#include "../math/RandomHelper.h"

namespace world {

Grass::Grass() : _rng(time(NULL)), _texture(32, 256, ImageType::RGB) {
    generateTexture();
}

void Grass::addBush(const vec3d &root) {
    std::uniform_real_distribution<double> angle(0, 2 * M_PI);
    std::uniform_real_distribution<double> dispersion(0, _dispersion);

    _points.emplace_back();
    auto &points = _points.back();

    _meshes.emplace_back();
    auto &mesh = _meshes.back();

    for (u32 i = 0; i < _grassCount; ++i) {
        double bladeAngle = angle(_rng);
        vec2d bladeDir(cos(bladeAngle), sin(bladeAngle));
        double bottomDist = dispersion(_rng);
        points.push_back(
            root + vec3d{bottomDist * bladeDir.x, bottomDist * bladeDir.y, 0});

        addBlade(root, points.back(), mesh);
    }
}

void Grass::removeAllBushes() {
    _points.clear();
    _meshes.clear();
}

std::vector<SceneNode> Grass::collectTemplates(ICollector &collector,
                                               const ExplorationContext &ctx) {
    std::vector<SceneNode> nodes;

    if (collector.hasChannel<Mesh>()) {
        auto &meshChan = collector.getChannel<Mesh>();

        ItemKey matKey;

        if (collector.hasChannel<Material>() && collector.hasChannel<Image>()) {
            auto &matChan = collector.getChannel<Material>();
            auto &imgChan = collector.getChannel<Image>();

            imgChan.put({"grass_texture"}, _texture, ctx);

            Material grassMat;
            grassMat.setKd(1, 1, 1);
            grassMat.setMapKd(ctx.mutateKey({"grass_texture"}).str());

            matKey = {"grass_material"};
            matChan.put(matKey, grassMat, ctx);
        }

        for (int i = 0; i < _points.size(); ++i) {
            ItemKey meshKey{NodeKeys::fromInt(i)};
            meshChan.put(meshKey, _meshes[i], ctx);

            nodes.push_back(ctx.createNode(meshKey, matKey));
        }
    }

    return nodes;
}

void Grass::collect(ICollector &collector,
                    const IResolutionModel &resolutionModel,
                    const ExplorationContext &ctx) {

    if (collector.hasChannel<SceneNode>()) {
        auto &objChan = collector.getChannel<SceneNode>();
        auto nodes = collectTemplates(collector, ctx);

        for (int i = 0; i < nodes.size(); ++i) {
            ItemKey nodeKey{NodeKeys::fromInt(i)};
            objChan.put(nodeKey, nodes[i]);
        }
    }
}

void Grass::addBlade(const vec3d &root, const vec3d &bottom, Mesh &mesh) {
    vec3d dir = bottom - root;
    const double distToRoot = dir.norm();
    const double bendFactor = (1 + distToRoot / _dispersion) / 2;
    const double topDist = abs(randScale(_rng, _bend, 1.15)) * bendFactor;
    dir /= distToRoot;
    vec3d topShift{topDist * dir.x, topDist * dir.y,
                   randScale(_rng, _height) - topDist};
    vec3d top = bottom + topShift;
    vec3d rightShift{dir.y * _width / 2, -dir.x * _width / 2, 0};

    BezierCurve curve{bottom, top, vec3d{0, 0, topShift.z} * 0.5,
                      vec3d{-topShift.x, -topShift.y, -topShift.z * 0.5} * 0.2};

    for (u32 j = 0; j <= _pointCount; ++j) {
        double t = static_cast<double>(j) / _pointCount;
        vec3d c = curve.getPointAt(t);
        vec3d d = curve.getDerivativeAt(t);
        vec3d normal = rightShift.crossProduct(d).normalize();

        mesh.newVertex(c - rightShift, normal, {0, t});
        mesh.newVertex(c + rightShift, normal, {1, t});

        if (j != 0) {
            int m = mesh.getVerticesCount();
            mesh.newFace(m - 4, m - 3, m - 2);
            mesh.newFace(m - 3, m - 1, m - 2);
        }
    }
}

void Grass::generateTexture() {
    // Pixel per pixel shading

    for (int y = 0; y < _texture.height(); ++y) {
        for (int x = 0; x < _texture.width(); ++x) {
            _texture.rgb(x, y).setf(0.4, 0.8, 0.2);
        }
    }
}

HabitatFeatures Grass::randomize() {
    // Generate characteristics first, then deduce habitat features from it
    std::geometric_distribution<int> count(0.045); // so that (1 - p) ^ 100 <
                                                   // 0.99
    _grassCount = count(_rng);
    _dispersion = randScale(_rng, 0.1, 1.5);
    _height = randScale(_rng, 0.1, 1.3);
    _width = randScale(_rng, 0.1 * _height, 1.5);
    auto bend = exponentialDistribFromMedian(_height / 2.0);
    _bend = bend(_rng);

    double surface = _width * _bend + 0.04;

    HabitatFeatures features{};
    features._sea = false;
    features._slope = {0, 70.0 / 180.0 * M_PI};
    std::uniform_real_distribution<double> altMax(200, 3000 - surface * 2000);
    features._altitude.y = altMax(_rng);
    std::uniform_real_distribution<double> altMin(
        0, max(0, features._altitude.y - 300));
    features._altitude.x = altMin(_rng);
    features._density = 0.1 / (surface * _grassCount);

    // Add bunch of variations
    removeAllBushes();

    for (int i = 0; i < 10; ++i) {
        addBush();
    }

    return features;
}

} // namespace world
