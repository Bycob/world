
#include "World.h"

namespace world {

template <typename T, typename... Args> T &World::addDecorator(Args... args) {
    T *decorator = new T(args...);
    addDecoratorInternal(decorator);
    return *decorator;
}

template <typename T, typename... Args>
T &World::addObject(const world::WorldZone &zone, Args... args) {
    return getChunk(zone).addObject<T>(args...);
}
} // namespace world