#ifndef WORLD_PRIVATEWORLDCOLLECTOR_H
#define WORLD_PRIVATEWORLDCOLLECTOR_H

#include <map>

#include "Collector.h"

namespace world {

    // Private classes : no WORLDAPI_EXPORT
    class PrivateCollectorItem {
    public:
        PrivateCollectorItem(const Object3D &object3D) : _object3D(object3D) {}

        Object3D _object3D;
    };

    class PrivateCollector {
    public:
        std::map<Collector::ItemKey, std::unique_ptr<CollectorItem>> _items;
    };

    class PrivateCollectorIterator {
    public:
        std::map<Collector::ItemKey, std::unique_ptr<CollectorItem>>::iterator _objectIt;
    };
}

#endif //WORLD_PRIVATEWORLDCOLLECTOR_H
