#ifndef WORLD_GRASS_H
#define WORLD_GRASS_H

#include "world/core/WorldConfig.h"

#include <random>
#include <vector>

#include "world/core/WorldTypes.h"
#include "world/core/WorldNode.h"
#include "world/math/Vector.h"
#include "world/assets/Mesh.h"
#include "world/assets/Image.h"

namespace world {

class WORLDAPI_EXPORT Grass : public WorldNode {
public:
    Grass();

    void addBush(const vec3d &root);

    std::vector<SceneNode> collectTemplates(ICollector &collector,
                                            const ExplorationContext &ctx);

    void collect(ICollector &collector, const IResolutionModel &resolutionModel,
                 const ExplorationContext &ctx) override;

private:
    std::mt19937_64 _rng;

    typedef std::vector<vec3d> GrassPoints;
    std::vector<GrassPoints> _points;
    std::vector<Mesh> _meshes;
    Image _texture;

    u32 _grassCount = 20;
    /// Number of points per grass blade
    u32 _pointCount = 4;
    /// Standard deviation of distance of the grass blades from the center of
    /// this node
    double _dispersion = 0.07;
    /// Maximum horizontal shift of the blade top
    double _bend = 0.1;
    double _height = 0.2;
    double _width = 0.02;


    void addBlade(const vec3d &root, const vec3d &bottom, Mesh &mesh);

    void generateTexture();
};

} // namespace world

#endif // WORLD_GRASS_H
