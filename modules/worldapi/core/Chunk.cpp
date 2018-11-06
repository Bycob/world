#include "Chunk.h"

#include <map>

#include "WorldObject.h"
#include "CollectorContextWrap.h"
#include "ResolutionModelContextWrap.h"

namespace world {

using ObjectKey = Chunk::ObjectKey;

class PChunk {
public:
    std::map<ObjectKey, std::unique_ptr<WorldObject>> _objects;
    int _counter = 0;
};

Chunk::Chunk(const vec3d &position, const vec3d &size)
        : _internal(new PChunk()), _offset(position), _size(size) {}

Chunk::~Chunk() { delete _internal; }

void Chunk::setResolutionLimits(double min, double max) {
    _minResolution = min;
    _maxResolution = max;
}

void Chunk::collect(ICollector &collector, const IResolutionModel &explorer) {
    CollectorContextWrap wcollector(collector);
    ResolutionModelContextWrap wexplorer(explorer);

    for (auto &object : _internal->_objects) {
        wcollector.setCurrentObject(object.first);
        wcollector.setOffset(object.second->getPosition3D());

        wexplorer.setOffset(object.second->getPosition3D());

        object.second->collect(wcollector, wexplorer);
    }
}

void Chunk::addObjectInternal(WorldObject *object) {
    _internal->_objects.emplace(_internal->_counter,
                                std::unique_ptr<WorldObject>(object));
    _internal->_counter++;
}
} // namespace world
