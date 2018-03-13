#ifndef WORLD_WORLDCOLLECTOR_H
#define WORLD_WORLDCOLLECTOR_H

#include <worldapi/worldapidef.h>

#include <vector>
#include <worldapi/Material.h>

#include "World.h"
#include "../Object3D.h"
#include "../Material.h"

template <typename T>
class WORLDAPI_EXPORT IWorldCollector {
public:
    virtual void collect(T &world, ChunkNode &chunkNode) = 0;
};

class WorldCollector;

class PrivateWorldCollectorIterator;

class WORLDAPI_EXPORT WorldCollectorIterator
        : public std::iterator<std::forward_iterator_tag, WorldCollector> {
public:
    WorldCollectorIterator(WorldCollector &collector);

private:

};

class PrivateWorldCollector;

class WORLDAPI_EXPORT WorldCollector : public IWorldCollector<World> {
public:
    typedef long ObjectKey;
    typedef long PartKey;

    WorldCollector();
    virtual ~WorldCollector();

    /** Delete all the resources harvested from the previous
     * "collect" calls */
    virtual void reset();

    /** Harvest all the resources in the given chunk. This includes
     * the object located in this chunk, and the object parts from
     * objects in parent chunks. */
    virtual void collect(World & world, ChunkNode & chunk);

    void addObject3D(ObjectKey objkey, PartKey partkey, const Object3D &object);
    void addMaterial(ObjectKey objkey, const Material &material);
protected:
    PrivateWorldCollector * _internal;
};


#endif //WORLD_WORLDCOLLECTOR_H
