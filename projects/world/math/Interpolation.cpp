#include "Interpolation.h"

namespace world {
const Interpolation::interpFunc Interpolation::LINEAR = [](double x) {
    return x;
};
const Interpolation::interpFunc Interpolation::COSINE = [](double x) {
    return (1 - cos(x * M_PI)) * 0.5;
};

double Interpolation::interpolateCosine(double x1, double y1, double x2,
                                        double y2, double x) {
    return interpolate(x1, y1, x2, y2, x, COSINE);
}

double Interpolation::interpolateLinear(double x1, double y1, double x2,
                                        double y2, double x) {
    return interpolate(x1, y1, x2, y2, x, LINEAR);
}

// hermite cubic interpolation from
// https://blog.demofox.org/2015/08/15/resizing-images-with-bicubic-interpolation/
// See wikipedia page for more details
double cuberp(double *v, double x) {
    const double x2 = x * x;
    const double x3 = x * x2;

    const double a = -v[0] / 2. + 3. * v[1] / 2. - 3. * v[2] / 2. + v[3] / 2.;
    const double b = v[0] - 5. * v[1] / 2. + 2. * v[2] - v[3] / 2.;
    const double c = -v[0] / 2. + v[2] / 2.;
    const double d = v[1];

    return a * x3 + b * x2 + c * x + d;
}
} // namespace world
