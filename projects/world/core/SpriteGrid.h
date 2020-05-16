#ifndef WORLD_SPRITEGRID_H
#define WORLD_SPRITEGRID_H

#include "WorldConfig.h"

#include "world/core/WorldTypes.h"
#include "world/math/BoundingBox.h"

namespace world {

class WORLDAPI_EXPORT SpriteGrid {
public:
    SpriteGrid(u32 count = 1);

    SpriteGrid(u32 rows, u32 cols);

    SpriteGrid(u32 count, u32 rows, u32 cols);

    u32 getCount() const;

    BoundingBox getBbox(u32 id) const;

private:
    u32 _count;
    u32 _rows, _cols;

    /** Returns number of sprite on a line if there are `count` squared sprites
     * on the image */
    static u32 getLineCount(u32 count);
};
} // namespace world

#endif // WORLD_SPRITEGRID_H
