#ifndef VKWORLD_BUSH
#define VKWORLD_BUSH

#include "VkWorldConfig.h"

#include <vector>

#include <world/core/WorldTypes.h>
#include <world/core/WorldNode.h>
#include <world/math/Vector.h>
#include <world/assets/Mesh.h>


namespace world {

class BushesPrivate;

/**
 *
 */
class VKWORLD_EXPORT Bushes : public WorldNode {
public:
    Bushes();

    ~Bushes();

    void addPosition(const vec3d &position);

    void collect(
        ICollector &collector, const IResolutionModel &resolutionModel,
        const ExplorationContext &ctx = ExplorationContext::getDefault());

private:
    BushesPrivate *_internal;

    std::vector<vec3d> _positions;

    u32 _count;
    double _height;
    double _groundSize;
    double _topSize;
    double _bladeWidth;
    u32 _bladeSegments;

    Mesh _mesh;


    void prepare();

    void updateData();

    void flush(ICollector &collector);
};

} // namespace world

#endif // VKWORLD_BUSH
