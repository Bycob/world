#ifndef WORLD_PRIVATEWORLDCOLLECTOR_H
#define WORLD_PRIVATEWORLDCOLLECTOR_H

#include <map>

#include "WorldCollector.h"

using ObjectKey = WorldCollector::ObjectKey;
using PartKey = WorldCollector::PartKey;

// Private classes : no WORLDAPI_EXPORT
class CollectorChunk;
class CollectorObject;

class CollectorObjectPart {
public:

};

class CollectorObject {
public:
    CollectorChunk& _parent;
    WorldObject& _object;
    std::map<PartKey, CollectorObjectPart> _keys;

    CollectorObject(CollectorChunk& parent, WorldObject& object);
};

class CollectorChunk {
public:
    ChunkNode &_chunkNode;
    std::map<ObjectKey, std::unique_ptr<CollectorObject>> _objects;

    CollectorChunk(ChunkNode& chunkNode);
};

class PrivateWorldCollector {
public:
    std::map<ChunkID, std::unique_ptr<CollectorChunk>> _chunks;
};


#endif //WORLD_PRIVATEWORLDCOLLECTOR_H
