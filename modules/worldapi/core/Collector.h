#ifndef WORLD_WORLDCOLLECTOR_H
#define WORLD_WORLDCOLLECTOR_H

#include "core/WorldConfig.h"

#include <vector>
#include <map>

#include "WorldTypes.h"
#include "assets/Object3D.h"
#include "assets/Material.h"
#include "assets/Scene.h"

#include "ICollector.h"

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

		void disableItem(const ItemKey &key) override;

		void addMaterial(const ItemKey &key, const Material &material) override;

        void addTexture(const ItemKey &key, const std::string &texName, const Image &texture) override;

        CollectorIterator iterateItems();

		void fillScene(Scene &scene);

    protected:
        PrivateCollector *_internal;

        friend class CollectorIterator;
    };

    class PrivateCollectorItem;

    class WORLDAPI_EXPORT CollectorItem {
    public:
        CollectorItem(const ICollector::ItemKey &key, const Object3D &object3D);

        ~CollectorItem();

        const Object3D &getObject3D() const;

        Object3D &getObject3D();

        /** */
		optional<const Material&> getMaterial(const std::string &key) const;

		typedef struct {
		    std::string _uid;
            const Image &_image;
		} texture;
		optional<texture> getTexture(const std::string &key) const;

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
