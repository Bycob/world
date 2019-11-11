#include "TileSystem.h"

namespace world {

TileSystemIterator::TileSystemIterator(const TileSystem &tileSystem,
                                       const IResolutionModel &resolutionModel,
                                       const BoundingBox &bounds)
        : _tileSystem(tileSystem), _resolutionModel(resolutionModel),
          _bounds(bounds) {

    // start at lod 0
    _min = _tileSystem.getTileCoordinates(_bounds.getLowerBound(), 0);
    _max = _tileSystem.getTileCoordinates(_bounds.getUpperBound(), 0);
    _current = _min;

    while (!_endReached && !isTileRequired(_current)) {
        _parents.push_back(_current);
        step();
    }
}

void TileSystemIterator::operator++() {
    while (!_endReached) {
        step();
        if (!isTileRequired(_current))
            _parents.push_back(_current);
        else break;
    }
}

TileCoordinates TileSystemIterator::operator*() { return _current; }

// TODO iterator end is poorly managed
bool TileSystemIterator::endReached() const { return _endReached; }

void TileSystemIterator::step() {
    if (_current._pos.z < _max._pos.z) {
        _current._pos.z++;
    } else {
        _current._pos.z = _min._pos.z;
        if (_current._pos.y < _max._pos.y) {
            _current._pos.y++;
        } else {
            _current._pos.y = _min._pos.y;
            if (_current._pos.x < _max._pos.x) {
                _current._pos.x++;
            } else {
                if (!_parents.empty()) {
                    startTile(_parents.front());
                    _parents.pop_front();
                }
                else {
                    _endReached = true;
                }
            }
        }
    }
}

void TileSystemIterator::startTile(TileCoordinates coords) {
    _min = { coords._pos * _tileSystem._factor, coords._lod + 1 };
    _max = _min;
    _max._pos += vec3i{_tileSystem._factor - 1};

    // if dim == 0 set max = 0
    auto eps = std::numeric_limits<double>::epsilon();
    if (abs(_tileSystem._baseSize.x) < eps)
        _max._pos.x = 0;
    if (abs(_tileSystem._baseSize.y) < eps)
        _max._pos.y = 0;
    if (abs(_tileSystem._baseSize.z) < eps)
        _max._pos.z = 0;

    _current = _min;
}

inline void expandDimension(BoundingBox &bbox) {
    auto lower = bbox.getLowerBound();
    auto upper = bbox.getUpperBound();
    auto size = bbox.getDimensions();

    if (abs(size.x) < std::numeric_limits<double>::epsilon()) {
        lower.x = -std::numeric_limits<double>::max();
        upper.x = std::numeric_limits<double>::max();
    }

    if (abs(size.y) < std::numeric_limits<double>::epsilon()) {
        lower.y = -std::numeric_limits<double>::max();
        upper.y = std::numeric_limits<double>::max();
    }

    if (abs(size.z) < std::numeric_limits<double>::epsilon()) {
        lower.z = -std::numeric_limits<double>::max();
        upper.z = std::numeric_limits<double>::max();
    }

    bbox.reset(lower, upper);
}

bool TileSystemIterator::isTileRequired(TileCoordinates coordinates) {
    vec3d lower = _tileSystem.getTileOffset(coordinates);
    vec3d upper = lower + _tileSystem.getTileSize(coordinates._lod);
    BoundingBox bbox{lower, upper};
    expandDimension(bbox);
    double resolutionInTile = _resolutionModel.getMaxResolutionIn(bbox);
    int refLod = _tileSystem.getLod(resolutionInTile);
    return coordinates._lod >= refLod;
}

} // namespace world