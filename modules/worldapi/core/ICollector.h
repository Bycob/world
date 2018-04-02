#ifndef WORLD_ICOLLECTOR_H
#define WORLD_ICOLLECTOR_H

#include "core/WorldConfig.h"

#include <tuple>

#include "WorldKeys.h"
#include "assets/Material.h"
#include "assets/Object3D.h"

namespace world {

    class WORLDAPI_EXPORT ICollector {
    public:
        typedef std::tuple<ChunkKey, ObjectKey, AssetKey> ItemKey;

		struct ItemKeys {
		    /** Generates a key from the world's perspective : we need to
		     * identify the chunk, the object and the part of the object.*/
			static ItemKey inWorld(const ChunkKey &chunkKey, const ObjectKey &objKey, const AssetKey &assetKey) {
				return std::make_tuple(chunkKey, objKey, assetKey);
			}

			/** Generates a key from inside a chunk. We just need to
			 * specify which object and which part of it we want a key for. */
			static ItemKey inChunk(const ObjectKey &objKey, const AssetKey &assetKey) {
				return inWorld(ChunkKeys::none(), objKey, assetKey);
			}

			/** Generates a key from inside an object. We just need to
			 * specify which part of the object we want a key for. */
			static ItemKey inObject(const AssetKey &assetKey) {
				return inWorld(ChunkKeys::none(), ObjectKeys::defaultKey(), assetKey);
			}
		};

        virtual void addItem(const ItemKey &key, const Object3D &object) = 0;

		virtual bool hasItem(const ItemKey &key) const = 0;

		virtual void removeItem(const ItemKey &key) = 0;

		virtual void addMaterial(const ItemKey &key, const Material &material) = 0;
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
