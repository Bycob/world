#ifndef WORLD_ICLONEABLE_H
#define WORLD_ICLONEABLE_H

template <typename T> class ICloneable {
public:
    virtual ~ICloneable() = default ;
    virtual T * clone() const = 0;
};

#endif //WORLD_ICLONEABLE_H
