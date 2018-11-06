#ifndef WORLD_WORLD_H
#define WORLD_WORLD_H

#include "core/WorldConfig.h"

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <functional>

#include "IChunkSystem.h"
#include "WorldObject.h"
#include "WorldFolder.h"
#include "IWorldDecorator.h"
#include "ICollector.h"

namespace world {

class World;

typedef IWorldDecorator<World> WorldDecorator;

class PWorld;

class WORLDAPI_EXPORT World {
public:
    /** Create a complete and rich world that can be used
     * as a demonstration of the API power ! */
    static World *createDemoWorld();

    World();

    World(const World &world) = delete;

    virtual ~World();

    template <typename T, typename... Args> T &addDecorator(Args... args);

    template <typename T, typename... Args>
    T &addObject(const WorldZone &zone, Args... args);

    // NAVIGATION
    std::vector<WorldZone> exploreNeighbours(const WorldZone &zone);

    WorldZone exploreLocation(const vec3d &location);

    std::vector<WorldZone> exploreInside(const WorldZone &zone);

    // ASSETS
    virtual void collect(const WorldZone &zone, ICollector &collector,
                         const IResolutionModel &explorer);

protected:
    virtual void onFirstExploration(const WorldZone &chunk);

private:
    PWorld *_internal;

    std::unique_ptr<IChunkSystem> _chunkSystem;
    // TODO remplacer ça par un ICache, qui peut être un
    // dossier, une interface réseau, rien...
    WorldFolder _directory;

    void addDecoratorInternal(WorldDecorator *decorator);

    Chunk &getChunk(const WorldZone &zone);
};
} // namespace world

#include "World.inl"

#endif // WORLD_WORLD_H