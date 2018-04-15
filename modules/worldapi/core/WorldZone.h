#ifndef WORLD_WORLDZONE_H
#define WORLD_WORLDZONE_H

#include "core/WorldConfig.h"

#include <memory>

#include "WorldTypes.h"
#include "math/Vector.h"
#include "WorldKeys.h"
#include "Chunk.h"

namespace world {

class WorldZone;

class WORLDAPI_EXPORT IWorldZoneHandler {
public:
    virtual IWorldZoneHandler *clone() const = 0;

    /** Gets an ID for the zone. This ID enable the chunk system
     * to recognize the zone among all the zone loaded. */
    virtual const ChunkKey &getID() const = 0;

    virtual Chunk &chunk() = 0;

    virtual const Chunk &getChunk() const = 0;

    virtual bool hasParent() const = 0;

    virtual optional<WorldZone> getParent() const = 0;

    vec3d getAbsoluteOffset() const;

    vec3d getRelativeOffset(const WorldZone &other) const;
};

// TODO rename "ChunkPointer" ?
class WORLDAPI_EXPORT WorldZone {
public:
    template <typename T, typename... Args>
    WorldZone(Args... args) : _handler(std::make_unique<T>(args...)) {}

    WorldZone(IWorldZoneHandler *handler) : _handler(handler) {}

    WorldZone(const WorldZone &other) : _handler(other._handler->clone()) {}

    WorldZone &operator=(const WorldZone &other) {
        _handler.reset(other._handler->clone());
        return *this;
    }

    bool operator<(const WorldZone &other) const {
        return _handler->getID() < other._handler->getID();
    }

    bool operator==(const WorldZone &other) const {
        return _handler->getID() == other._handler->getID();
    }

    // TODO return const IWorldZoneHandler*
    IWorldZoneHandler *operator->() const { return _handler.get(); };

private:
    std::unique_ptr<IWorldZoneHandler> _handler;
};

} // namespace world

#endif // WORLD_WORLDZONE_H
