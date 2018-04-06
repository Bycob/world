#ifndef WORLD_PRIVATEWORLDCOLLECTOR_H
#define WORLD_PRIVATEWORLDCOLLECTOR_H

#include <map>
#include <set>

#include "core/Memory.h"
#include "Collector.h"

namespace world {

    // Private classes : no WORLDAPI_EXPORT
    class PrivateCollectorItem {
    public:
        PrivateCollectorItem(const ICollector::ItemKey &myKey, const Object3D &object3D)
                : _myKey(myKey), _object3D(object3D) {}

        ICollector::ItemKey _myKey;
        Object3D _object3D;
		std::map<std::string, Material> _materials;
		std::map<std::string, ConstRefOrValue<Image>> _image;
    };

    class PrivateCollector {
    public:
        std::map<Collector::ItemKey, std::unique_ptr<CollectorItem>> _items;
        std::set<Collector::ItemKey> _disabled;
    };

    class PrivateCollectorIterator {
    public:
        std::map<Collector::ItemKey, std::unique_ptr<CollectorItem>>::iterator _objectIt;
    };
}

#endif //WORLD_PRIVATEWORLDCOLLECTOR_H
