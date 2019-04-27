#ifndef WORLD_WORLD_H
#define WORLD_WORLD_H

#include "world/core/WorldConfig.h"

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <functional>

#include "IChunkSystem.h"
#include "WorldNode.h"
#include "WorldFolder.h"
#include "IChunkDecorator.h"
#include "ICollector.h"

#define MAX_PRIMARY_NODES 1024

namespace world {

class WorldPrivate;

class WORLDAPI_EXPORT World {
public:
    /** Create a complete and rich world that can be used
     * as a demonstration of the API capabilities. */
    static World *createDemoWorld();


    World();

    World(const World &world) = delete;

    virtual ~World();

    /** Adds a node to the world. The node will be explored after all
     * the primary nodes added previously.
     *
     * The world is (arbitrarily) limited in the number of primary nodes
     * you can have, because exploration of primary nodes is not efficient
     * if there are many. If you want to add a lot of nodes to the world,
     * consider use a ChunkSystem or an other Node container. */
    template <typename T, typename... Args>
    T &addPrimaryNode(const vec3d &position, Args &... args);

    // ASSETS
    virtual void collect(ICollector &collector,
                         const IResolutionModel &resolutionModel);

protected:
    void addPrimaryNodeInternal(WorldNode *node);

    /** Gets initial environment to initialize the base context */
    virtual IEnvironment *getInitialEnvironment();

private:
    WorldPrivate *_internal;

    // TODO remplacer ça par un ICache, qui peut être un
    // dossier, une interface réseau, rien...
    WorldFolder _directory;
};
} // namespace world

#include "World.inl"

#endif // WORLD_WORLD_H