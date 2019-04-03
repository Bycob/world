#pragma once

#include "world/core/WorldConfig.h"

#include "world/math/Vector.h"
#include "world/math/MathsHelper.h"
#include "ICollector.h"
#include "IResolutionModel.h"

namespace world {

    class WorldNodePrivate;

class WORLDAPI_EXPORT WorldNode {
public:
    WorldNode();

    virtual ~WorldNode();

    void setPosition3D(const vec3d &pos);

    vec3d getPosition3D() const { return _position; }

    void collectAll(ICollector &collector, double resolution);

    virtual void collect(ICollector &collector,
                         const IResolutionModel &resolutionModel);


    template <typename T, typename... Args> T &addChild(Args... args);
protected:
    WorldNodePrivate* _internal;

    vec3d _position;

    void collectChild(const NodeKey &key, WorldNode &child, ICollector &collector, const IResolutionModel &resolutionModel);

private:
    void addChildInternal(WorldNode *child);
};

template <typename T, typename... Args> T &WorldNode::addChild(Args... args) {
    T *node = new T(args...);
    addChildInternal(node);
    return *node;
}

} // namespace world