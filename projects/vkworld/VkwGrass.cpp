#include "VkwGrass.h"

#include <world/math/Bezier.h>

#include "VkwTextureGenerator.h"

namespace world {

class VkwGrassPrivate {
public:
    std::unique_ptr<VkwTextureGenerator> _generator;
};

VkwGrass::VkwGrass() : _internal(new VkwGrassPrivate()) {}

VkwGrass::~VkwGrass() { delete _internal; }

void VkwGrass::collectSelf(ICollector &collector,
                           const IResolutionModel &resolutionModel,
                           const ExplorationContext &ctx) {
    if (!_isInitialized) {
        setup();
        _isInitialized = true;
    }

    if (collector.hasChannel<Image>()) {
        auto &imgChan = collector.getChannel<Image>();
        imgChan.put(ctx.mutateKey({"0"}),
                    _internal->_generator->generateTexture());
    }
}

void VkwGrass::setup() {
    u32 size = 256;
    _internal->_generator =
        std::make_unique<VkwTextureGenerator>(size, size, "grass.frag");
    auto &generator = *_internal->_generator;
    Mesh &mesh = generator.mesh();

    // Setup mesh
    vec3d origin{0, -1, 0};
    vec3d target{-0.4, 0.4, 0};
    vec3d weight1{0, 1, 0};
    vec3d weight2{0.5, -0.5, 0};
    BezierCurve curve(origin, target, weight1, weight2);
    const u32 subdiv = 5;

    for (u32 i = 0; i <= subdiv; ++i) {
        double t = static_cast<double>(i) / subdiv;
        vec3d base = curve.getPointAt(t);
        vec3d deriv = curve.getDerivativeAt(t);
        deriv = vec3d{deriv.y, -deriv.x, 0}.normalize();
        double dist = 0.03;

        const u32 first = mesh.getVerticesCount();
        const u32 line = 3;
        mesh.newVertex(base - deriv * dist);
        mesh.newVertex(base);
        mesh.newVertex(base + deriv * dist);

        if (i != subdiv) {
            mesh.newFace(first, first + 1, first + line + 1);
            mesh.newFace(first, first + line + 1, first + line);
            mesh.newFace(first + 1, first + 2, first + line + 2);
            mesh.newFace(first + 1, first + line + 2, first + line + 1);
        }
    }
}
} // namespace world
