
#ifndef EXPLORATION_CONTEXT_H
#define EXPLORATION_CONTEXT_H

#include "world/core/WorldConfig.h"

#include "WorldKeys.h"
#include "world/math/Vector.h"

namespace world {

class WORLDAPI_EXPORT ExplorationContext {
public:
    static ExplorationContext &getDefault();


    ExplorationContext();

    void addOffset(const vec3d &offset);

    void appendPrefix(const NodeKey &prefix);

    ItemKey mutateKey(const ItemKey &key) const;

    vec3d getOffset() const;
private:
    ItemKey _keyPrefix;
    vec3d _offset;
};

}

#endif //EXPLORATION_CONTEXT_H
