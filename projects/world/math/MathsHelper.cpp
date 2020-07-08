#include "MathsHelper.h"

namespace world {

double wrapAngle(double angle) {
    while (angle < -M_PI)
        angle += M_PI * 2;
    while (angle > M_PI)
        angle -= M_PI * 2;
    return angle;
}

} // namespace world
