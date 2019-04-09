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

    /** Collects the object according to given resolution model, and store
     * result in `collector`.
     *
     * A context can be passed as a parameter. The context hold information
     * about the current collect operation and can change the collected data
     * to fit them in the big picture.
     *
     * The "collect" method does not take care of the position of the node.
     * If you want the object to be collected at its correct location, you
     * have to add an offset to the context.
     *
     * The default implementation of this method calls #collectSelf to collect
     * this node, then calls #collectChild on every child. */
    virtual void collect(
        ICollector &collector, const IResolutionModel &resolutionModel,
        const ExplorationContext &ctx = ExplorationContext::getDefault());


    template <typename T, typename... Args> T &addChild(Args &&... args);

protected:
    WorldNodePrivate *_internal;

    vec3d _position;

    virtual void collectSelf(ICollector &collector,
                             const IResolutionModel &resolutionModel,
                             const ExplorationContext &ctx);

    void collectChildren(ICollector &collector,
                         const IResolutionModel &resolutionModel,
                         const ExplorationContext &ctx);

    void collectChild(const NodeKey &key, WorldNode &child,
                      ICollector &collector,
                      const IResolutionModel &resolutionModel,
                      const ExplorationContext &ctx);

private:
    void addChildInternal(WorldNode *child);
};

template <typename T, typename... Args>
T &WorldNode::addChild(Args &&... args) {
    T *node = new T(args...);
    addChildInternal(node);
    return *node;
}

} // namespace world