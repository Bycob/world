#ifndef WORLD_COLLECTORCONTEXTWRAP_H
#define WORLD_COLLECTORCONTEXTWRAP_H

#include <worldapi/worldapidef.h>

#include "../maths/Vector.h"
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

    protected:
        void addItem(const ItemKey &key, Object3D &object) override;

    private:
        std::pair<bool, ChunkKey> _currentChunk;
        std::pair<bool, ObjectKey> _currentObject;

        vec3d _offset;
    };
}

#endif //WORLD_COLLECTORCONTEXTWRAP_H
