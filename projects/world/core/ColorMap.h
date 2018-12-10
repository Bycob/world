#ifndef WORLD_COLORMAP_H
#define WORLD_COLORMAP_H

#include "world/core/WorldConfig.h"

#include <vector>
#include <utility>

#include <armadillo/armadillo>

#include "world/assets/Color.h"
#include "world/assets/Image.h"
#include "world/math/Vector.h"
#include "world/math/Interpolation.h"

namespace world {

/** A ColorMap is a multidimensional space in which each point corresponds to
 * a color.
 * To configure this color map, you just have to specify several pairs of
 * spatial position and color. Then a spatial interpolation is used to fill all
 * the colormap. Then it's possible to retrieve the value at a special point
 * calling the method getColorAt(). All the coordinates are bounded to [0, 1].
 * At the moment this colormap only supports 2 dimensions. */
class WORLDAPI_EXPORT ColorMap {
public:
    typedef vec2d position;
    typedef vec3d color;

    ColorMap(const vec2i &resolution);

    void setOrder(int order);

    void addPoint(const vec2d &pos, const Color4d &color);

    void rebuild();

    /** Gets the color at the given point.
     * @param pos A location on the color map. All the coordinates should be
     * between 0 and 1. */
    Color4d getColorAt(const vec2d &pos);

    Image *createImage();

private:
    int _order = 3;
    std::vector<std::pair<position, color>> _points;

    arma::cube _cache;
    bool _shouldRebuild;

    color toInternalColor(const Color4d &color);
    Color4d toColor4d(const color &color);
};
} // namespace world

#endif // WORLD_COLORMAP_H