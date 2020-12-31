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
#include "IChunkDecorator.h"
#include "ICollector.h"
#include "WorldFile.h"

#define MAX_PRIMARY_NODES 1024

namespace world {

class WorldPrivate;

class WORLDAPI_EXPORT World : public ISerializable {
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

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

    void setCacheDirectory(const std::string &path);

protected:
    NodeCache _cacheRoot;


    void addPrimaryNodeInternal(WorldNode *node);

    /** Gets initial environment to initialize the base context */
    virtual IEnvironment *getInitialEnvironment();

private:
    WorldPrivate *_internal;
};
} // namespace world

#include "World.inl"

#endif // WORLD_WORLD_H