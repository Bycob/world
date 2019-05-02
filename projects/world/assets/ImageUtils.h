#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#include "world/core/WorldConfig.h"

#include "Image.h"
#include "Color.h"
#include "world/math/Vector.h"

namespace world {

class WORLDAPI_EXPORT ImageUtils {
public:
    ImageUtils() = delete;

    /**
     * \param dstPos position of the top-left corner of the source image on
     * the destination image. {0., 0.} is top left and {1., 1.} is bottom right.
     * dstPos can be outside of destination image bounds.
     * \param size of the source image relatively to the destination image. */
    static void paintTexturef(Image &dst, const Image &src, const vec2d &dstPos,
                              const vec2d &dstSize);

    static void fill(Image &img, const Color4d &color);

    /** Draw a line at the given coordinates (in pixels) */
    static void drawLine(Image &img, const vec2d &from, const vec2d &to,
                         double width, const Color4d &color,
                         bool softEnds = false);

private:
};

} // namespace world

#endif // IMAGE_UTILS_H
