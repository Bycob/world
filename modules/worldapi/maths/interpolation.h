
#ifndef WORLD_INTERPOLATION_H
#define WORLD_INTERPOLATION_H

#include <worldapi/worldapidef.h>

namespace maths {
    double WORLDAPI_EXPORT interpolate(double x1, double y1, double x2, double y2, double x);
    double WORLDAPI_EXPORT interpolateLinear(double x1, double y1, double x2, double y2, double x);
}

#endif //WORLD_INTERPOLATION_H
