
#ifndef EXPLORATION_CONTEXT_H
#define EXPLORATION_CONTEXT_H

#include "world/core/WorldConfig.h"

#include "WorldKeys.h"
#include "world/math/Vector.h"
#include "IEnvironment.h"

namespace world {

class WORLDAPI_EXPORT ExplorationContext {
public:
    static const ExplorationContext &getDefault();


    ExplorationContext();

    void addOffset(const vec3d &offset);

    void appendPrefix(const NodeKey &prefix);

    void setEnvironment(IEnvironment *environment);

    ItemKey mutateKey(const ItemKey &key) const;

    /// Handy alias for #mutateKey
    ItemKey operator()(const ItemKey &key) const;

    vec3d getOffset() const;

    bool hasEnvironment() const;

    const IEnvironment &getEnvironment() const;

private:
    ItemKey _keyPrefix;
    vec3d _offset;

    IEnvironment *_environment;
};

} // namespace world

#endif // EXPLORATION_CONTEXT_H
