#pragma once

#include "core/WorldConfig.h"

#include <tuple>
#include <memory>
#include <iterator>
#include <functional>

#include "math/BoundingBox.h"
#include "math/MathsHelper.h"

#include "ICollector.h"
#include "WorldObject.h"

namespace world {

class PChunk;

/**  */
class WORLDAPI_EXPORT Chunk {
public:
    typedef int ObjectKey;

    Chunk(const vec3d &offset, const vec3d &size);

    Chunk(const Chunk &chunk) = delete;

    virtual ~Chunk();

    void setResolutionLimits(double min, double max);

    double getMinResolution() const { return _minResolution; }

    double getMaxResolution() const { return _maxResolution; }

    const vec3d &getSize() const { return _size; }

    const vec3d &getOffset() const { return _offset; }

    template <typename T, typename... Args> T &addObject(Args... args);

    void collect(ICollector &collector, const IResolutionModel &explorer);

private:
    PChunk *_internal;

    double _minResolution = 0;
    double _maxResolution = 1e100;
    vec3d _offset;
    vec3d _size;

    void addObjectInternal(WorldObject *object);
};

template <typename T, typename... Args> T &Chunk::addObject(Args... args) {
    T *object = new T(args...);
    addObjectInternal(object);
    return *object;
}
} // namespace world
