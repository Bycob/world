#include "ColorMap.h"

namespace world {

ColorMap::ColorMap(const vec2i &res)
        : _cache(res.x, res.y, 3), _shouldRebuild(true) {}

void ColorMap::addPoint(const vec2d &pos, const Color4d &color) {
    _points.emplace_back(pos, toInternalColor(color));

    _shouldRebuild = true;
}

void ColorMap::rebuild() {
    // We scale one dimension to have a coherent map
    auto w = _cache.n_rows;
    auto h = _cache.n_cols;
    double scale = (double)h / w;

    IDWInterpolator<position, color> interp(3);

    for (auto &point : _points) {
        position scaled{point.first.x, point.first.y * scale};
        interp.addData(scaled, point.second);
    }

    for (arma::uword x = 0; x < w; x++) {
        for (arma::uword y = 0; y < h; y++) {
            vec2d pt{(double)x / (w - 1), (double)y / (w - 1)};
            auto data = interp.getData(pt);

            _cache(x, y, 0) = data.x;
            _cache(x, y, 1) = data.y;
            _cache(x, y, 2) = data.z;
        }
    }

    _shouldRebuild = false;
}

Color4d ColorMap::getColorAt(const vec2d &pos) {
    if (_shouldRebuild) {
        rebuild();
    }

    auto ix = static_cast<arma::uword>(pos.x * (_cache.n_rows - 1));
    auto iy = static_cast<arma::uword>(pos.y * (_cache.n_cols - 1));

    return Color4d(_cache(ix, iy, 0), _cache(ix, iy, 1), _cache(ix, iy, 2));
}

Image *ColorMap::createImage() {
    if (_shouldRebuild) {
        rebuild();
    }

    return new Image(_cache);
}

ColorMap::color ColorMap::toInternalColor(const Color4d &color) {
    return {color._r, color._g, color._b};
}

Color4d ColorMap::toColor4d(const color &color) { return Color4d(); }
} // namespace world
