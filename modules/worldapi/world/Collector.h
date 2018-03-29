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

    class CollectorIterator;

    class PrivateCollector;

    class WORLDAPI_EXPORT Collector : public ICollector {
    public:
        Collector();

        virtual ~Collector();

        /** Delete all the resources harvested from the previous
         * "collect" calls */
        virtual void reset();

        void addItem(const ItemKey &key, const Object3D &item) override;

		bool hasItem(const ItemKey &key) const override;

		void removeItem(const ItemKey &key) override;

        CollectorIterator iterateItems();

    protected:
        PrivateCollector *_internal;

        friend class CollectorIterator;
    };

    class PrivateCollectorItem;

    class WORLDAPI_EXPORT CollectorItem {
    public:
        CollectorItem(const Object3D &object3D);

        ~CollectorItem();

        const Object3D &getObject3D() const;

        Object3D &getObject3D();

    private:
        PrivateCollectorItem *_internal;

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

        std::pair<Collector::ItemKey, CollectorItem* > operator*();

        bool hasNext() const;

    private:
        PrivateCollectorIterator *_internal;

        Collector &_collector;
    };
}

#endif //WORLD_WORLDCOLLECTOR_H
