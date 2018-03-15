#ifndef WORLD_WORLDZONE_H
#define WORLD_WORLDZONE_H

#include <worldapi/worldapidef.h>

#include <memory>

#include "../maths/Vector.h"
#include "ChunkID.h"

class Chunk;
class WorldZone;

class WORLDAPI_EXPORT IWorldZoneHandler {
public:
    virtual Chunk& chunk() = 0;
    virtual const Chunk& getChunk() const = 0;
    // TODO Le chunkID est un type trop specifique pour l'usage reserve ici. changer en long UID ?
    virtual const ChunkID &getID() const = 0;

    virtual bool hasParent() = 0;
    virtual WorldZone getParent() const = 0;

    virtual std::pair<WorldZone, bool> getOrCreateNeighbourZone(const maths::vec3i &direction) = 0;
    virtual std::vector<std::pair<WorldZone, bool>> getOrCreateChildren() = 0;

    virtual maths::vec3d getAbsoluteOffset() = 0;

    virtual IWorldZoneHandler* clone() = 0;
};

// TODO rename "ChunkPointer" ?
class WORLDAPI_EXPORT WorldZone {
public:
    template <typename T, typename... Args>
    WorldZone(Args...args) : _handler(std::make_unique<T>(args...)) {}
    WorldZone(IWorldZoneHandler * handler) : _handler(handler) {}
    WorldZone(const WorldZone & other) : _handler(other._handler->clone()) {}

    WorldZone& operator=(const WorldZone &other) {
        _handler.reset(other._handler->clone());
    }

    bool operator<(const WorldZone &other) const { return _handler->getID() < other._handler->getID(); }
    bool operator==(const WorldZone &other) const {return _handler->getID() == other._handler->getID(); }
    // TODO return const IWorldZoneHandler*
    IWorldZoneHandler* operator->() const {return _handler.get(); };
private:
    std::unique_ptr<IWorldZoneHandler> _handler;
};


#endif //WORLD_WORLDZONE_H
