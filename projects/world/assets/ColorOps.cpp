#include "ColorOps.h"

#include "world/math/MathsHelper.h"

namespace world {
Color4d ColorOps::superpose(const Color4d &bg, const Color4d &fg) {
    double a = bg._a + (1 - bg._a) * fg._a;
    return {world::mix(bg._r, fg._r, fg._a), world::mix(bg._g, fg._g, fg._a),
            world::mix(bg._b, fg._b, fg._a), a};
}

Color4d ColorOps::mix(const Color4d &c1, const Color4d &c2, double x) {
    return {world::mix(c1._r, c2._r, x), world::mix(c1._g, c2._g, x),
            world::mix(c1._b, c2._b, x), world::mix(c1._a, c2._a, x)};
}

Color4d ColorOps::fromHSV(double h, double s, double v, double a) {
    h *= 6;
    double r = clamp(abs(h - 3) - 1, 0, 1);
    double g = clamp(abs(modd(h - 2, 6.) - 3) - 1, 0, 1);
    double b = clamp(abs(modd(h - 4, 6.) - 3) - 1, 0, 1);
    const double c = v * (1 - s);
    return {world::mix(c, v, r), world::mix(c, v, g), world::mix(c, v, b), a};
}
} // namespace world