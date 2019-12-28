#include "VkwGrass.h"

#include <world/math/Bezier.h>
#include <world/math/RandomHelper.h>

#include "wrappers/VkwTextureGenerator.h"
#include "wrappers/VkwRandomTexture.h"

namespace world {

class VkwGrassPrivate {
public:
    std::unique_ptr<VkwTextureGenerator> _generator;
    Mesh _mesh;
    Image _finalTexture{1, 1, ImageType::GREYSCALE};
};

VkwGrass::VkwGrass()
        : _internal(new VkwGrassPrivate()), _rng(std::random_device()()) {}

VkwGrass::~VkwGrass() { delete _internal; }

std::vector<SceneNode> VkwGrass::collectTemplates(
    ICollector &collector, const ExplorationContext &ctx) {
    if (!_isInitialized) {
        setup();
        _isInitialized = true;
    }

    std::vector<SceneNode> nodes;

    if (collector.hasChannel<Mesh>()) {
        auto &meshChan = collector.getChannel<Mesh>();

        ItemKey matKey;

        if (collector.hasChannel<Material>() && collector.hasChannel<Image>()) {
            auto &matChan = collector.getChannel<Material>();
            auto &imgChan = collector.getChannel<Image>();

            imgChan.put({"texturea"}, _internal->_finalTexture, ctx);

            Material grassMat;
            grassMat.setKd(1, 1, 1);
            grassMat.setMapKd(ctx({"texturea"}).str());

            matKey = {"material"};
            matChan.put(matKey, grassMat, ctx);
        }

        ItemKey meshKey{"mesh"};
        meshChan.put(meshKey, _internal->_mesh, ctx);

        nodes.push_back(ctx.createNode(meshKey, matKey));
    }

    return nodes;
}

HabitatFeatures VkwGrass::randomize() { return HabitatFeatures{}; }

void VkwGrass::collectSelf(ICollector &collector,
                           const IResolutionModel &resolutionModel,
                           const ExplorationContext &ctx) {

    auto nodes = collectTemplates(collector, ctx);

    if (collector.hasChannel<SceneNode>()) {
        auto &nodeChan = collector.getChannel<SceneNode>();

        nodeChan.put({"item"}, nodes.at(0), ctx);
    }
}

double grassWidth(double t, double da = 0, double db = -1, double a = 1,
                  double b = 0) {
    double l = 2 * a - 2 * b + da + db;
    double m = -3 * a + 3 * b - 2 * da - db;
    return l * (t * t * t) + m * (t * t) + da * t + a;
}

void VkwGrass::setup() {
    u32 size = 512;
    _internal->_generator =
        std::make_unique<VkwTextureGenerator>(size, size, "grass.frag");
    auto &generator = *_internal->_generator;

    VkwRandomTexture randTexture;
    vec3f grassColor(0.48, 0.96, 0.34);
    generator.addParameter(0, DescriptorType::UNIFORM_BUFFER,
                           MemoryUsage::CPU_READS, sizeof(grassColor),
                           &grassColor);
    generator.addImageParameter(1, randTexture.get());

    double pheight = _height / _size;
    double pwidth = _width / _size;
    std::uniform_real_distribution<double> basePosDistrib(-1, 1);
    std::uniform_real_distribution<double> targetHeightDistrib(
        pheight * 2 * 0.6, pheight * 2);
    std::uniform_int_distribution<int> senseDistrib(0, 1);

    Mesh &mesh = generator.mesh();

    // Setup texture mesh
    for (u32 j = 0; j < _count; ++j) {
        double height = targetHeightDistrib(_rng);
        double bend = randScale(_rng, _bend) * height;
        double sense = (senseDistrib(_rng) ? 1 : -1);
        // From 0 to 1, indicates how grouped are the blades in the image, in
        // case of strong bend
        double regroup = 0.8;
        vec3d origin{basePosDistrib(_rng) * (1 - bend * regroup) -
                         bend * sense * (1 - regroup),
                     -1, 0};
        vec3d target{origin.x + bend * sense, origin.y + height - bend, 0};

        // TODO document this thing with schemas
        double b = origin.length(target) / 2;
        double a = (target.y - origin.y) / 2;
        double c = b * b / a;
        vec3d weight1{0, _fold * c, 0};
        vec3d weight2 = (origin + vec3d{0, c, 0} - target) * _fold;
        BezierCurve curve(origin, target, weight1, weight2);
        const u32 subdiv = 5;

        for (u32 i = 0; i <= subdiv; ++i) {
            double t = static_cast<double>(i) / subdiv;
            vec3d base = curve.getPointAt(t);
            vec3d deriv = curve.getDerivativeAt(t);
            deriv = vec3d{deriv.y, -deriv.x, 0}.normalize();
            double width = pwidth * grassWidth(t, 0, 0);

            const u32 first = mesh.getVerticesCount();
            const u32 line = 3;
            mesh.newVertex(base - deriv * width, {0, 0, 1}, {-1, t});
            mesh.newVertex(base, {0, 0, 1}, {0, t});
            mesh.newVertex(base + deriv * width, {0, 0, 1}, {1, t});

            if (i != subdiv) {
                mesh.newFace(first, first + 1, first + line + 1);
                mesh.newFace(first, first + line + 1, first + line);
                mesh.newFace(first + 1, first + 2, first + line + 2);
                mesh.newFace(first + 1, first + line + 2, first + line + 1);
            }
        }
    }

    // Setup final mesh
    double hs = _size / (2 * std::sqrt(2));
    Mesh &finalMesh = _internal->_mesh;
    vec3d norm1{-std::sqrt(2) / 2, -std::sqrt(2) / 2, 0};
    finalMesh.newVertex({-hs, hs, _size}, norm1, {0, 1});
    finalMesh.newVertex({hs, -hs, _size}, norm1, {1, 1});
    finalMesh.newVertex({hs, -hs, 0}, norm1, {1, 0});
    finalMesh.newVertex({-hs, hs, 0}, norm1, {0, 0});
    finalMesh.addFace({0, 2, 1});
    finalMesh.addFace({0, 3, 2});

    vec3d norm2{std::sqrt(2) / 2, -std::sqrt(2) / 2, 0};
    finalMesh.newVertex({-hs, -hs, _size}, norm2, {0, 1});
    finalMesh.newVertex({hs, hs, _size}, norm2, {1, 1});
    finalMesh.newVertex({hs, hs, 0}, norm2, {1, 0});
    finalMesh.newVertex({-hs, -hs, 0}, norm2, {0, 0});
    finalMesh.addFace({4, 6, 5});
    finalMesh.addFace({4, 7, 6});

    _internal->_finalTexture = generator.generateTexture();
}
} // namespace world
