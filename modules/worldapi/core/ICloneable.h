#ifndef WORLD_ICLONEABLE_H
#define WORLD_ICLONEABLE_H

namespace world {

    template<typename T>
    class ICloneable {
    public:

        virtual T *clone() const = 0;
    };
}

#endif //WORLD_ICLONEABLE_H
