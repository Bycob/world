#ifndef WORLD_COLOR_H
#define WORLD_COLOR_H

#include <random>

#include "world/core/WorldTypes.h"
#include "world/math/MathsHelper.h"

namespace world {
struct Color4u;
struct Color4d;

struct Color4u {
    // A8R8G8B8
    u32 _color;

    static u32 assemble(u32 r, u32 g, u32 b, u32 a) {
        return ((a & 0xff) << 24) | ((r & 0xff) << 16) | ((g & 0xff) << 8) |
               (b & 0xff);
    }

    Color4u() : _color(0xff000000) {}
    Color4u(u32 rgba) : _color(rgba) {}
    Color4u(u32 r, u32 g, u32 b, u32 a = 255) : _color(assemble(r, g, b, a)) {}

    void set(u32 r, u32 g, u32 b) { _color = assemble(r, g, b, getAlpha()); }
    void set(u32 r, u32 g, u32 b, u32 a) { _color = assemble(r, g, b, a); }

    void setAlpha(u32 a) { _color = (_color & 0xffffff) | ((a & 0xff) << 24); }
    void setRed(u32 r) { _color = (_color & 0xff00ffff) | ((r & 0xff) << 16); }
    void setGreen(u32 g) { _color = (_color & 0xffff00ff) | ((g & 0xff) << 8); }
    void setBlue(u32 b) { _color = (_color & 0xffffff00) | (b & 0xff); }
    u32 getAlpha() const { return _color >> 24; }
    u32 getRed() const { return (_color >> 16) & 0xff; }
    u32 getGreen() const { return (_color >> 8) & 0xff; }
    u32 getBlue() const { return _color & 0xff; }
    u32 getARGB() const { return _color; }

    operator Color4d() const;
};

struct Color4d {
    double _r, _g, _b, _a;

    Color4d(double r, double g, double b, double a = 1)
            : _r(r), _g(g), _b(b), _a(a) {}
    Color4d() : Color4d(0, 0, 0) {}

    void set(double r, double g, double b) {
        _r = r;
        _g = g;
        _b = b;
    }
    void set(double r, double g, double b, double a) {
        _r = r;
        _g = g;
        _b = b;
        _a = a;
    }
    operator Color4u() const;
};

inline Color4u::operator Color4d() const {
    return {getRed() / 255.0, getGreen() / 255.0, getBlue() / 255.0,
            getAlpha() / 255.0};
}

inline Color4d::operator Color4u() const {
    return {static_cast<u32>(_r * 255), static_cast<u32>(_g * 255),
            static_cast<u32>(_b * 255), static_cast<u32>(_a * 255)};
}

// ===== Color manipulation

template <class RNG>
inline Color4d jitter(RNG &rng, const Color4d &color, double factor) {
    std::normal_distribution<double> distrib(0, factor);
    return {clamp(color._r + distrib(rng), 0, 1),
            clamp(color._g + distrib(rng), 0, 1),
            clamp(color._b + distrib(rng), 0, 1), color._a};
}

} // namespace world

#endif // WORLD_COLOR_H
