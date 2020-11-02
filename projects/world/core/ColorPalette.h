
#ifndef WORLD_COLORPALETTE_H
#define WORLD_COLORPALETTE_H

#include "world/core/WorldConfig.h"

#include "world/math/Vector.h"
#include "world/assets/Image.h"
#include "WorldFile.h"

namespace world {

class WORLDAPI_EXPORT ColorPalette : public ISerializable {
public:
    static ColorPalette fromStartEnd(const vec3d &start, const vec3d &end);

    /** color = a + b * cos(2 * pi * (c * x + d)) */
    ColorPalette(const vec3d &a, const vec3d &b, const vec3d &c,
                 const vec3d &d);

    Color4d getColor(double x) const;

    void exportToImage(Image &dest);

    void write(WorldFile &wf) const override;

    void read(const WorldFile &wf) override;

private:
    vec3d _a, _b, _c, _d;
};

} // namespace world

#endif // WORLD_COLORPALETTE_H
