#include "Chunk.h"

#include <map>

#include "WorldNode.h"
#include "WorldNode_p.h"
#include "CollectorContextWrap.h"
#include "ResolutionModelContextWrap.h"

namespace world {

Chunk::Chunk(const vec3d &position, const vec3d &size)
        : _offset(position), _size(size) {}

Chunk::~Chunk() { delete _internal; }

void Chunk::setResolutionLimits(double min, double max) {
    _minResolution = min;
    _maxResolution = max;
}

void Chunk::collect(ICollector &collector, const IResolutionModel &explorer) {
    CollectorContextWrap wcollector(collector);
    ResolutionModelContextWrap wexplorer(explorer);

    for (auto &object : _internal->_children) {
        wcollector.setKeyPrefix(ItemKeys::root(object.first));
        wcollector.setOffset(object.second->getPosition3D());

        wexplorer.setOffset(object.second->getPosition3D());

        object.second->collect(wcollector, wexplorer);
    }
}

} // namespace world
