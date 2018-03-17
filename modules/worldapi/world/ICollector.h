#ifndef WORLD_ICOLLECTOR_H
#define WORLD_ICOLLECTOR_H

#include <worldapi/worldapidef.h>

namespace world {

    template<typename T>
    class WORLDAPI_EXPORT ICollector{
            public:
            virtual void collect(T &world, WorldZone &zone) = 0;
    };
}

#endif //WORLD_ICOLLECTOR_H
