#include "Interpolation.h"

#include <math.h>

namespace world {
    const Interpolation::interpFunc Interpolation::LINEAR = [](double x) {return x;};
    const Interpolation::interpFunc Interpolation::COSINE = [](double x) {return (1 - cos(x * M_PI)) * 0.5;};

    double Interpolation::interpolateCosine(double x1, double y1, double x2, double y2, double x) {
		return interpolate(x1, y1, x2, y2, x, COSINE);
    }

    double Interpolation::interpolateLinear(double x1, double y1, double x2, double y2, double x) {
		return interpolate(x1, y1, x2, y2, x, LINEAR);
    }
}
