
#ifndef WORLD_COLORPALETTE_H
#define WORLD_COLORPALETTE_H

#include "world/core/WorldConfig.h"

#include "world/math/Vector.h"
#include "world/assets/Image.h"

namespace world {

class WORLDAPI_EXPORT ColorPalette {
public:
    ColorPalette(const vec3d &a, const vec3d &b, const vec3d &c,
                 const vec3d &d);

    Color4d getColor(double x) const;

    void exportToImage(Image &dest);

private:
    vec3d _a, _b, _c, _d;
};

} // namespace world

#endif // WORLD_COLORPALETTE_H
