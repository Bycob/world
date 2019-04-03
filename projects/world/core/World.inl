
#include "World.h"

namespace world {

template <typename T, typename... Args>
T &World::addPrimaryNode(const vec3d &position, Args... args) {
    T *node = new T(args...);
    node->setPosition3D(position);
    addPrimaryNodeInternal(node);
    return *node;
}
} // namespace world