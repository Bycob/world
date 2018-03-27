#ifndef WORLD_ICOLLECTOR_H
#define WORLD_ICOLLECTOR_H

#include <worldapi/worldapidef.h>

#include <tuple>

#include "WorldKeys.h"

namespace world {

    class Object3D;

    class WORLDAPI_EXPORT ICollector {
    public:
        typedef std::tuple<ChunkKey, ObjectKey, AssetKey> ItemKey;

        virtual void addItem(const ItemKey &key, const Object3D &object) = 0;

        virtual void addItem(const ChunkKey &chunkId, const ObjectKey &objId, const AssetKey &partId, const Object3D &object) {
            addItem(ItemKey(chunkId, objId, partId), object);
        }

        virtual void addItem(const ObjectKey &objId, const AssetKey &partId, const Object3D &object) {
            addItem(ItemKey(ChunkKeys::none(), objId, partId), object);
        }

        virtual void addItem(const AssetKey &partId, const Object3D &object) {
            addItem(ItemKey(ChunkKeys::none(), ObjectKeys::defaultKey(), partId), object);
        }
    protected:
        void passItemTo(ICollector &collector, const ItemKey &key, Object3D &object) {
            collector.addItem(key, object);
        }

        virtual void addItem(const ItemKey &key, Object3D &object) {
            addItem(key, const_cast<const Object3D &>(object));
        }
    };
}

#endif //WORLD_ICOLLECTOR_H
