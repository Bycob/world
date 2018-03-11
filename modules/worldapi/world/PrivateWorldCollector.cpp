#include "PrivateWorldCollector.h"


CollectorChunk::CollectorChunk(ChunkNode &chunkNode)
        : _chunkNode(chunkNode) {

}

CollectorObject::CollectorObject(CollectorChunk &parent, WorldObject &object)
        : _parent(parent), _object(object) {

}