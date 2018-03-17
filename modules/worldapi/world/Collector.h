#ifndef WORLD_WORLDCOLLECTOR_H
#define WORLD_WORLDCOLLECTOR_H

#include <worldapi/worldapidef.h>

#include <vector>
#include <map>

#include "World.h"
#include "ICollector.h"
#include "worldapi/assets/Object3D.h"
#include "worldapi/assets/Material.h"


namespace world {

    class CollectorObject;

    class CollectorObjectPart;

    class CollectorIterator;

    class PrivateCollectorChunk;

    class PrivateCollector;

    class WORLDAPI_EXPORT Collector : public ICollector<World> {
    public:
        typedef ChunkID ChunkKey;
        typedef std::pair<ChunkKey, long> ObjectKey;
        typedef long PartKey;

        Collector();

        virtual ~Collector();

        /** Delete all the resources harvested from the previous
         * "collect" calls */
        virtual void reset();

        /** Harvest all the resources in the given chunk. This includes
         * the object located in this chunk, and the object parts from
         * objects in parent chunks.
         * If the collector has already collected the zone, it won't collect
         * it again. Call the resetZone() method to make a zone collectable
         * again (or even the reset() method)*/
        virtual void collect(World &world, WorldZone &zone);

        CollectorObject &getCollectedObject(const ObjectKey &key);

        CollectorIterator iterateObjects();

    protected:
        PrivateCollector *_internal;

        ChunkKey getChunkKey(ObjectKey key);

        friend class CollectorIterator;
    };

    class PrivateCollectorObject;

    class WORLDAPI_EXPORT CollectorObject {
    public:
        CollectorObject(WorldZone &zone, WorldObject &object);

        ~CollectorObject();

        void putPart(const Collector::PartKey &key, const Object3D &object);

        CollectorObjectPart &getPart(const Collector::PartKey &key);

        const Object3D &getPartAsObject3D(const Collector::PartKey &key) const;

    private:
        PrivateCollectorObject *_internal;

        friend class Collector;
    };

    class PrivateCollectorObjectPart;

    class WORLDAPI_EXPORT CollectorObjectPart {
    public:
        CollectorObjectPart(const Object3D &object3D);

        ~CollectorObjectPart();

        const Object3D &getObject3D() const;

        Object3D &getObject3D();

    private:
        PrivateCollectorObjectPart *_internal;

        friend class Collector;
    };

    class PrivateCollectorIterator;

    class WORLDAPI_EXPORT CollectorIterator
            : public std::iterator<std::forward_iterator_tag, Collector> {
    public:
        CollectorIterator(Collector &collector);

        CollectorIterator(const CollectorIterator &other);

        ~CollectorIterator();

        void operator++();

        std::pair<Collector::ObjectKey, CollectorObject *> operator*();

        bool hasNext() const;

    private:
        PrivateCollectorIterator *_internal;

        Collector &_collector;

        std::map<Collector::ObjectKey, std::unique_ptr<CollectorObject>> &objects() const;

        std::map<Collector::ChunkKey, std::unique_ptr<PrivateCollectorChunk>> &chunks() const;
    };
}

#endif //WORLD_WORLDCOLLECTOR_H
