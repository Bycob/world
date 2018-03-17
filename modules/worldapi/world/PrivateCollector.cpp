#include "PrivateCollector.h"

namespace world {

    PrivateCollectorChunk::PrivateCollectorChunk(const WorldZone &chunkNode)
            : _chunkNode(chunkNode), _nextKey(0) {

    }

    PrivateCollectorObject::PrivateCollectorObject(const WorldZone &zone, WorldObject &object)
            : _zone(zone), _object(object) {

    }
}