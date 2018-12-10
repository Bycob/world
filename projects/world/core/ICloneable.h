#ifndef WORLD_ICLONEABLE_H
#define WORLD_ICLONEABLE_H

namespace world {

template <typename T> class ICloneable {
public:
    virtual T *clone() const = 0;
};
} // namespace world

#endif // WORLD_ICLONEABLE_H
