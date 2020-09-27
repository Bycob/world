#include "ColorPalette.h"

namespace world {

ColorPalette::ColorPalette(const vec3d &a, const vec3d &b, const vec3d &c,
                           const vec3d &d)
        : _a(a), _b(b), _c(c), _d(d) {}

Color4d ColorPalette::getColor(double x) const {
    auto r = _a + _b * ((_c * x + _d) * 2. * M_PI).cos();
    return {r.x, r.y, r.z};
}

void ColorPalette::exportToImage(Image &dest) {
    for (int y = 0; y < dest.height(); ++y) {
        for (int x = 0; x < dest.width(); ++x) {
            dest.rgb(x, y) = getColor(double(x) / (dest.width() - 1));
        }
    }
}
} // namespace world