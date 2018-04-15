#ifndef WORLD_PRIVATEWORLDCOLLECTOR_H
#define WORLD_PRIVATEWORLDCOLLECTOR_H

#include <map>
#include <set>

#include "core/Memory.h"
#include "Collector.h"

namespace world {

// Private classes : no WORLDAPI_EXPORT
class PCollectorItem {
public:
    PCollectorItem(const ICollector::ItemKey &myKey, const Object3D &object3D)
            : _myKey(myKey), _object3D(object3D) {}

    ICollector::ItemKey _myKey;
    Object3D _object3D;
    std::map<std::string, Material> _materials;
    std::map<std::string, std::unique_ptr<Image>> _image;
};

class PCollector {
public:
    std::map<Collector::ItemKey, std::unique_ptr<CollectorItem>> _items;
    std::set<Collector::ItemKey> _disabled;
};

class PCollectorIterator {
public:
    std::map<Collector::ItemKey, std::unique_ptr<CollectorItem>>::iterator
        _objectIt;
};
} // namespace world

#endif // WORLD_PRIVATEWORLDCOLLECTOR_H
