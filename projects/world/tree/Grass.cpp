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

void Grass::collect(ICollector &collector,
                    const IResolutionModel &resolutionModel,
                    const ExplorationContext &ctx) {

    if (collector.hasChannel<SceneNode>() && collector.hasChannel<Mesh>()) {
        auto &objChan = collector.getChannel<SceneNode>();
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
            matChan.put(matKey, grassMat);
        }

        for (int key = 0; key < _points.size(); ++key) {
            ItemKey itemKey{NodeKeys::fromInt(key)};
            meshChan.put(itemKey, _meshes[key], ctx);

            SceneNode obj = ctx.createNode(itemKey, matKey);
            objChan.put(itemKey, obj, ctx);
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

} // namespace world
