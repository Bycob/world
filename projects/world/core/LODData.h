#pragma once

#include "world/core/WorldConfig.h"

#include "world/math/Vector.h"

namespace world {

class WORLDAPI_EXPORT LODData {
public:
    LODData();

    LODData(const vec3d &chunkSize, double maxResolution = 0.2);

    const vec3d &getChunkSize() const { return _chunkSize; }

    double getMaxResolution() const { return _maxResolution; }

private:
    vec3d _chunkSize;
    double _maxResolution;
};
} // namespace world
