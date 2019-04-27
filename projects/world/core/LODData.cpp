#include "LODData.h"

#include "world/math/MathsHelper.h"

namespace world {

LODData::LODData() : _chunkSize({0, 0, 0}) {}

LODData::LODData(const vec3d &chunkSize, double maxResolution)
        : _chunkSize(chunkSize), _maxResolution(maxResolution) {}

} // namespace world
