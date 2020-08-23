#ifndef WORLD_FLATMAPPER_H
#define WORLD_FLATMAPPER_H

#include "world/core/WorldConfig.h"

#include "FlatWorld.h"
#include "world/core/ColorMap.h"

namespace world {

/** A FlatMapper can create a map from a FlatWorld  */
class WORLDAPI_EXPORT FlatMapper {
public:
    FlatMapper();

    void map(FlatWorld &world, Image &output, const BoundingBox &bounds);

    ColorMap &terrainColors();

    void setZBounds(double min, double max);

    void setupDefaultColorScheme();

private:
    /// Wether or not terrain should be displayed on the map
    bool _terrain = true;
    /// Color of the terrain for the different altitudes
    ColorMap _terrainColors;
    /// Minimum and maximum altitude of the terrain
    vec2d _zbounds;
};

} // namespace world

#endif // WORLD_FLATMAPPER_H
