#ifndef WORLD_LEAF_PAINTER_H
#define WORLD_LEAF_PAINTER_H

#include "core/WorldConfig.h"

#include "assets/Image.h"

namespace world {

class WORLDAPI_EXPORT LeafPainter {
public:
    LeafPainter();

    void paint(Image &image);

private:
};

} // namespace world

#endif // WORLD_LEAF_PAINTER_H
