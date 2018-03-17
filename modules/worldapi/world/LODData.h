#pragma once

#include <worldapi/worldapidef.h>

#include "../maths/Vector.h"

namespace world {

    class WORLDAPI_EXPORT LODData {
    public:
        LODData();

        LODData(const vec3d &chunkSize);

        LODData(const LODData &other);

        virtual ~LODData();

        const vec3d &getChunkSize() const { return _chunkSize; }

        double getMinDetailSize() const { return _minDetailSize; }

    private:
        vec3d _chunkSize;
        double _minDetailSize;
    };
}
