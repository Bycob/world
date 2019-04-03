
#ifndef EXPLORATION_CONTEXT_H
#define EXPLORATION_CONTEXT_H

#include "world/core/WorldConfig.h"

#include "WorldKeys.h"
#include "world/math/Vector.h"

namespace world {

class WORLDAPI_EXPORT ExplorationContext {
public:
    ExplorationContext();


private:
    ItemKey _keyPrefix;
    vec3d _offset;
};

}

#endif //EXPLORATION_CONTEXT_H
