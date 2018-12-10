#include "TileSystem.h"

namespace world {

TileSystemIterator::TileSystemIterator(const TileSystem &tileSystem,
                                       const IResolutionModel &resolutionModel,
                                       const WorldZone &zone)
        : _tileSystem(tileSystem), _resolutionModel(resolutionModel) {

    vec3d lower = zone.getInfo().getAbsoluteOffset();
    _bounds = {lower, lower + zone.getInfo().getDimensions()};

    // Start at lod 0
    startLod(0);

    while (!isTileRequired(_current) && !_endReached)
        step();
}

void TileSystemIterator::operator++() {
    BoundingBox tileBbox;

    do {
        step();
    } while (!isTileRequired(_current) && !_endReached);
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
                int newLod = _current._lod + 1;
                if (newLod <= _tileSystem._maxLod) {
                    startLod(newLod);
                } else {
                    _endReached = true;
                }
            }
        }
    }
}

void TileSystemIterator::startLod(int lod) {
    _min = _tileSystem.getTileCoordinates(_bounds.getLowerBound(), lod);
    _max = _tileSystem.getTileCoordinates(_bounds.getUpperBound(), lod);
    _current = _min;
}

inline void expandDimension(BoundingBox &bbox) {}

bool TileSystemIterator::isTileRequired(TileCoordinates coordinates) {
    vec3d lower = _tileSystem.getTileOffset(coordinates);
    vec3d upper = lower + _tileSystem.getTileSize(coordinates._lod);
    double resolutionInTile =
        _resolutionModel.getMaxResolutionIn({lower, upper});
    int refLod = _tileSystem.getLod(resolutionInTile);

    if (coordinates._lod >= refLod) {
        while (coordinates._lod > 0) {
            coordinates = _tileSystem.getParentTileCoordinates(coordinates);

            if (isTileRequired(coordinates)) {
                return false;
            }
        }
        return true;
    } else {
        return false;
    }
}

} // namespace world