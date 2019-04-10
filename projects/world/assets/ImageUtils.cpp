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
} // namespace world
