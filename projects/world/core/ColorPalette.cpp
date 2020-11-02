#include "ColorPalette.h"

namespace world {

ColorPalette ColorPalette::fromStartEnd(const vec3d &start, const vec3d &end) {
    vec3d med = (start + end) / 2;
    vec3d diff = start - med;
    return {med, diff, {0.5}, {0}};
}

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

void ColorPalette::write(WorldFile &wf) const {
    wf.addStruct("a", _a);
    wf.addStruct("b", _b);
    wf.addStruct("c", _c);
    wf.addStruct("d", _d);
}

void ColorPalette::read(const WorldFile &wf) {
    wf.readStructOpt("a", _a);
    wf.readStructOpt("b", _b);
    wf.readStructOpt("c", _c);
    wf.readStructOpt("d", _d);
}
} // namespace world