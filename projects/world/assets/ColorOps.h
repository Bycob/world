#ifndef WORLD_COLOROPS_H
#define WORLD_COLOROPS_H

#include "Color.h"

namespace world {

class ColorOps {
public:
    static Color4d superpose(const Color4d &bg, const Color4d &fg);

    static Color4d mix(const Color4d &c1, const Color4d &c2, double x);

    static Color4d fromHSV(double h, double s, double v, double a = 1);
};
} // namespace world

#endif // WORLD_COLOROPS_H
