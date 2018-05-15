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
    virtual ~IWorldZoneHandler() = default;

    /** Gets an ID for the zone. This ID enable the chunk system
     * to recognize the zone among all the zones loaded. */
    virtual ChunkKey getID() const = 0;

    /** Gets parent WorldZone if it exists. Otherwise, returns nullopt. */
    virtual optional<WorldZone> getParent() const = 0;

    /** Gets this WorldZone offset relative to parent WorldZone. */
    virtual vec3d getParentOffset() const = 0;

    /** Gets absolute offset in the world, in double precision.
     * This method may give bad results when the world gets very
     * large. */
    vec3d getAbsoluteOffset() const;

    /** Gets relative offset to an other WorldZone.  */
    vec3d getRelativeOffset(const WorldZone &other) const;

    virtual double getMaxResolution() const = 0;

    virtual double getMinResolution() const = 0;

    virtual vec3d getDimensions() const = 0;
};

// TODO rename "ChunkPointer" ?
class WORLDAPI_EXPORT WorldZone {
public:
    WorldZone(const ChunkKey &id, IWorldZoneHandler *handler)
            : _id(id), _handler(std::shared_ptr<IWorldZoneHandler>(handler)) {}

    bool operator<(const WorldZone &other) const { return _id < other._id; }

    bool operator==(const WorldZone &other) const { return _id == _id; }

    // TODO const
    IWorldZoneHandler &getInfo() const { return *_handler; }

    // TODO return const IWorldZoneHandler*
    IWorldZoneHandler *operator->() const { return _handler.get(); };

private:
    ChunkKey _id;
    std::shared_ptr<IWorldZoneHandler> _handler;
};

} // namespace world

#endif // WORLD_WORLDZONE_H
