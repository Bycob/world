#include "Chunk.h"

#include <map>

#include "WorldObject.h"
#include "CollectorContextWrap.h"

namespace world {

	using ObjectKey = Chunk::ObjectKey;

	class PrivateChunk {
	public:
		std::map<ObjectKey, std::unique_ptr<WorldObject>> _objects;
        int _counter = 0;
	};

	Chunk::Chunk(const vec3d &position, const vec3d &size)
			: _internal(new PrivateChunk()),
			  _offset(position),
			  _size(size) {

	}

	Chunk::~Chunk() {
		delete _internal;
	}

	void Chunk::setResolutionLimits(double min, double max) {
		_minResolution = min;
		_maxResolution = max;
	}

	void Chunk::addObject(WorldObject *object) {
		addObjectInternal(object);
	}

    void Chunk::collectWholeChunk(ICollector &collector) {
		CollectorContextWrap wcollector(collector);

        for (auto &object : _internal->_objects) {
            wcollector.setCurrentObject(object.first);
            wcollector.setOffset(object.second->getPosition3D());

			object.second->collectWholeObject(wcollector);
        }
    }

    void Chunk::collectPart(const BoundingBox &bbox, double maxDetailSize, ICollector &collector) {

    }

	void Chunk::addObjectInternal(WorldObject *object) {
		_internal->_objects.emplace(_internal->_counter, std::unique_ptr<WorldObject>(object));
        _internal->_counter++;
	}
}
