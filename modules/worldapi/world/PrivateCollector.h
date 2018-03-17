#ifndef WORLD_PRIVATEWORLDCOLLECTOR_H
#define WORLD_PRIVATEWORLDCOLLECTOR_H

#include <map>

#include "Collector.h"

namespace world {

    using ObjectKey = Collector::ObjectKey;
    using PartKey = Collector::PartKey;

    // Private classes : no WORLDAPI_EXPORT
    class PrivateCollectorChunk;

    class PrivateCollectorObjectPart {
    public:
        PrivateCollectorObjectPart(const Object3D &object3D) : _object3D(object3D) {}

        Object3D _object3D;
    };

    class PrivateCollectorObject {
    public:
        WorldZone _zone;
        WorldObject &_object;
        std::map<PartKey, std::unique_ptr<CollectorObjectPart>> _parts;

        PrivateCollectorObject(const WorldZone &, WorldObject &object);
    };

    class PrivateCollectorChunk {
    public:
        WorldZone _chunkNode;
        std::map<ObjectKey, std::unique_ptr<CollectorObject>> _objects;
        long _nextKey;

        PrivateCollectorChunk(const WorldZone &chunkNode);
    };

    class PrivateCollector {
    public:
        std::map<ChunkID, std::unique_ptr<PrivateCollectorChunk>> _chunks;
    };

    class PrivateCollectorIterator {
    public:
        std::map<ChunkID, std::unique_ptr<PrivateCollectorChunk>>::iterator _chunkIt;
        std::map<ObjectKey, std::unique_ptr<CollectorObject>>::iterator _objectIt;

    };
}

#endif //WORLD_PRIVATEWORLDCOLLECTOR_H
