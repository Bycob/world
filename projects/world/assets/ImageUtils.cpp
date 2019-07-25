#include "ImageUtils.h"

namespace world {

void ImageUtils::paintTexturef(Image &dst, const Image &src,
                               const vec2d &dstPos, const vec2d &dstSize) {
    vec2i dstDims{dst.width(), dst.height()};
    vec2i srcDims{src.width(), src.height()};
    vec2d startPoint = dstPos * dstDims;
    vec2d dims = dstSize * dstDims;
    vec2d endPoint = startPoint + dims;

    vec2i dstMin{static_cast<int>(ceil(max(startPoint.x, 0))),
                 static_cast<int>(ceil(max(startPoint.y, 0)))};
    vec2i dstMax{static_cast<int>(floor(min(endPoint.x, dstDims.x))),
                 static_cast<int>(floor(min(endPoint.y, dstDims.y)))};

    for (int x = dstMin.x; x < dstMax.x; ++x) {
        for (int y = dstMin.y; y < dstMax.y; ++y) {
            vec2d srcPos = (vec2d(x, y) - startPoint) / dims *
                           (srcDims - vec2d(0.01, 0.01));

            // TODO handle other image types
            // TODO test this function carefully
            dst.rgb(x, y) = src.rgb(static_cast<int>(floor(srcPos.x)),
                                    static_cast<int>(floor(srcPos.y)));
        }
    }
}

void ImageUtils::fill(Image &img, const Color4d &color) {
    for (u32 y = 0; y < img.height(); ++y) {
        for (u32 x = 0; x < img.width(); ++x) {
            img.rgb(x, y).setf(color._r, color._g, color._b);
        }
    }
}

void ImageUtils::drawLine(Image &img, const vec2d &from, const vec2d &to,
                          double width, const Color4d &color, bool softEnds) {
    vec2d dir = to - from;
    bool xbased = abs(dir.x) > abs(dir.y);
    double xmin, xmax, a, b;
    int imax, jmax;
    vec2d lower, higher;

    if (xbased) {
        if (from.x < to.x) {
            lower = from;
            higher = to;
        } else {
            higher = from;
            lower = to;
            dir = dir * -1;
        }
        xmin = lower.x;
        xmax = higher.x;
        a = (lower.y - higher.y) / (lower.x - higher.x);
        b = lower.y - a * xmin;

        imax = img.width() - 1;
        jmax = img.height() - 1;
    } else {
        if (from.y < to.y) {
            lower = from;
            higher = to;
        } else {
            higher = from;
            lower = to;
            dir = dir * -1;
        }
        xmin = lower.y;
        xmax = higher.y;
        a = (lower.x - higher.x) / (lower.y - higher.y);
        b = lower.x - a * xmin;

        imax = img.height() - 1;
        jmax = img.width() - 1;
    }

    const double hwidth = width / 2.0;
    const double endsWidth = softEnds ? hwidth : 0;
    const double lineLength = dir.norm();
    vec2d unit = dir.normalize();

    for (int i = int(floor(xmin - endsWidth)); i <= int(ceil(xmax + endsWidth));
         ++i) {
        if (i < 0 || i > imax)
            continue;

        const double mid = a * i + b;

        for (int j = int(floor(mid - hwidth)); j <= int(ceil(mid + hwidth));
             ++j) {
            if (j < 0 || j > jmax)
                continue;

            vec2i uv;
            if (xbased) {
                uv = {i, j};
            } else {
                uv = {j, i};
            }

            vec2d vec = uv - lower;
            const double dp = unit.dotProduct(vec);
            const double t = dp / lineLength;

            if ((t >= 0 && t <= 1) || softEnds) {
                double d;

                if (t < 0) {
                    d = lower.length(uv);
                } else if (t > 1) {
                    d = higher.length(uv);
                } else {
                    d = (vec - unit * dp).norm();
                }
                double f = clamp(d - hwidth, 0, 1);
                double f_1 = 1 - f;

                auto &initRgb = img.rgb(uv.x, uv.y);
                initRgb.setf(initRgb.getRedf() * f + color._r * f_1,
                             initRgb.getGreenf() * f + color._g * f_1,
                             initRgb.getBluef() * f + color._b * f_1);
            }
        }
    }
}
} // namespace world
