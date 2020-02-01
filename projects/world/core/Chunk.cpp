#include "Chunk.h"

#include <map>

#include "WorldNode.h"
#include "WorldNode_p.h"

namespace world {

Chunk::Chunk(const vec3d &size, double minRes, double maxRes)
        : _size(size), _minResolution(minRes), _maxResolution(maxRes) {}

void Chunk::setResolutionLimits(double min, double max) {
    _minResolution = min;
    _maxResolution = max;
}

} // namespace world
