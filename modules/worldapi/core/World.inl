
#include "World.h"

namespace world {

template <typename T, typename... Args> T &World::addDecorator(Args... args) {
    T *decorator = new T(args...);
    addDecoratorInternal(decorator);
    return *decorator;
}
} // namespace world