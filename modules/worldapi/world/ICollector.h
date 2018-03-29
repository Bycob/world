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

		struct ItemKeys {
			static ItemKey from(const ChunkKey &chunkKey, const ObjectKey &objKey, const AssetKey &assetKey) {
				return std::make_tuple(chunkKey, objKey, assetKey);
			}

			static ItemKey from(const ObjectKey &objKey, const AssetKey &assetKey) {
				return from(ChunkKeys::none(), objKey, assetKey);
			}

			static ItemKey from(const AssetKey &assetKey) {
				return from(ChunkKeys::none(), ObjectKeys::defaultKey(), assetKey);
			}
		};

        virtual void addItem(const ItemKey &key, const Object3D &object) = 0;

		virtual bool hasItem(const ItemKey &key) const = 0;

		virtual void removeItem(const ItemKey &key) = 0;
    protected:
        void passItemTo(ICollector &collector, const ItemKey &key, Object3D &object) {
            collector.addItemUnsafe(key, object);
        }

        virtual void addItemUnsafe(const ItemKey &key, Object3D &object) {
            addItem(key, object);
        }
    };
}

#endif //WORLD_ICOLLECTOR_H
