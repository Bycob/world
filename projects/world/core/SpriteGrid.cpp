#include "SpriteGrid.h"

namespace world {

SpriteGrid::SpriteGrid(u32 count)
        : _count(count), _rows(getLineCount(count)),
          _cols(getLineCount(count)) {}

SpriteGrid::SpriteGrid(u32 rows, u32 cols)
        : _count(rows * cols), _rows(rows), _cols(cols) {}

SpriteGrid::SpriteGrid(u32 count, u32 rows, u32 cols)
        : _count(count), _rows(rows), _cols(cols) {}

u32 SpriteGrid::getCount() const { return _count; }

BoundingBox SpriteGrid::getBbox(u32 id) const {
    u32 x = id % _rows;
    u32 y = id / _rows;
    vec3d o(double(x) / _rows, double(y) / _cols, 0);

    BoundingBox bbox(o, o + vec3d{1. / _rows, 1. / _cols, 0});
    return bbox;
}

u32 SpriteGrid::getLineCount(u32 count) {
    u32 line = 0;
    while (line * line < count) {
        ++line;
    }
    return line;
}
} // namespace world