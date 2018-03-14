#ifndef WORLD_WORLDCOLLECTOR_H
#define WORLD_WORLDCOLLECTOR_H

#include <worldapi/worldapidef.h>

#include <vector>
#include <map>
#include <worldapi/Material.h>

#include "World.h"
#include "../Object3D.h"
#include "../Material.h"

template <typename T>
class WORLDAPI_EXPORT IWorldCollector {
public:
    virtual void collect(T &world, WorldZone &chunkNode) = 0;
};


class CollectorObject;
class CollectorObjectPart;
class CollectorIterator;
class PrivateCollectorChunk;

class PrivateWorldCollector;

class WORLDAPI_EXPORT WorldCollector : public IWorldCollector<World> {
public:
    typedef ChunkID ChunkKey;
    typedef std::pair<ChunkKey, long> ObjectKey;
    typedef long PartKey;

    WorldCollector();
    virtual ~WorldCollector();

    /** Delete all the resources harvested from the previous
     * "collect" calls */
    virtual void reset();

    /** Harvest all the resources in the given chunk. This includes
     * the object located in this chunk, and the object parts from
     * objects in parent chunks.
     * If the collector has already collected the zone, it won't collect
     * it again. Call the resetZone() method to make a zone collectable
     * again (or even the reset() method)*/
    virtual void collect(World & world, WorldZone & zone);

    CollectorObject &getCollectedObject(const ObjectKey & key);
    CollectorIterator iterateObjects();
protected:
    std::unique_ptr<PrivateWorldCollector> _internal;

    ChunkKey getChunkKey(ObjectKey key);

    friend class CollectorIterator;
};

class PrivateCollectorObject;

class WORLDAPI_EXPORT CollectorObject {
public:
    CollectorObject(WorldZone &zone, WorldObject &object);

    void putPart(const WorldCollector::PartKey &key, const Object3D &object);

    CollectorObjectPart &getPart(const WorldCollector::PartKey &key);
    const Object3D &getPartAsObject3D(const WorldCollector::PartKey &key) const;
private:
    std::unique_ptr<PrivateCollectorObject> _internal;

    friend class WorldCollector;
};

class PrivateCollectorObjectPart;

class WORLDAPI_EXPORT CollectorObjectPart {
public:
    CollectorObjectPart(const Object3D &object3D);

    const Object3D &getObject3D() const;
    Object3D &getObject3D();
private:
    std::unique_ptr<PrivateCollectorObjectPart> _internal;

    friend class WorldCollector;
};

class PrivateCollectorIterator;

class WORLDAPI_EXPORT CollectorIterator
        : public std::iterator<std::forward_iterator_tag, WorldCollector> {
public:
    CollectorIterator(WorldCollector &collector);
    CollectorIterator(const CollectorIterator &other);
    ~CollectorIterator();

    void operator++();
    std::pair<WorldCollector::ObjectKey, CollectorObject*> operator*();

    bool hasNext() const;
private:
    std::unique_ptr<PrivateCollectorIterator> _internal;

    WorldCollector &_collector;

    std::map<WorldCollector::ObjectKey, std::unique_ptr<CollectorObject>> &objects() const;
    std::map<WorldCollector::ChunkKey, std::unique_ptr<PrivateCollectorChunk>> &chunks() const;
};


#endif //WORLD_WORLDCOLLECTOR_H
