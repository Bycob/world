#ifndef WORLD_COLLECTORCONTEXTWRAP_H
#define WORLD_COLLECTORCONTEXTWRAP_H

#include "core/WorldConfig.h"

#include "math/Vector.h"
#include "ICollector.h"

namespace world {

class WORLDAPI_EXPORT CollectorContextWrap : public ICollector {
public:
    ICollector &_collector;

    CollectorContextWrap(ICollector &wrapped);

    void setCurrentChunk(ChunkKey key);

    void setCurrentObject(ObjectKey key);

    void setOffset(const vec3d &offset);

    void addItem(const ItemKey &key, const Object3D &object) override;

    void removeItem(const ItemKey &key) override;

    void disableItem(const ItemKey &key) override;

    bool hasItem(const ItemKey &key) const override;

    void addMaterial(const ItemKey &key, const Material &material) override;

    void addTexture(const ItemKey &key, const std::string &texName,
                    const Image &texture) override;

protected:
    void addItemUnsafe(const ItemKey &key, Object3D &object) override;

private:
    std::pair<bool, ChunkKey> _currentChunk;
    std::pair<bool, ObjectKey> _currentObject;

    vec3d _offset;

    ItemKey mutateKey(const ItemKey &key) const;
};
} // namespace world

#endif // WORLD_COLLECTORCONTEXTWRAP_H
