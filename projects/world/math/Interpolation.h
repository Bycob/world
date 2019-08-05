
#ifndef WORLD_INTERPOLATION_H
#define WORLD_INTERPOLATION_H

#include "world/core/WorldConfig.h"

#include <utility>
#include <functional>

#include "MathsHelper.h"

namespace world {

struct Interpolation {
    typedef std::function<double(double)> interpFunc;

    static inline double interpolate(double x1, double y1, double x2, double y2,
                                     double x, const interpFunc &f) {
        double d = x2 - x1;

        if (d < std::numeric_limits<double>::epsilon()) {
            return y1;
        }

        double xFunc = f(clamp((x - x1) / d, 0, 1));
        // double yFunc = 6 * pow(xFunc, 5) - 15 * pow(xFunc, 4) + 10 *
        // pow(xFunc, 3);
        return y2 * xFunc + y1 * (1 - xFunc);
    }

    static const interpFunc LINEAR;
    static const interpFunc COSINE;

    static double interpolateCosine(double x1, double y1, double x2, double y2,
                                    double x);
    static double interpolateLinear(double x1, double y1, double x2, double y2,
                                    double x);
};

/** Hermite cubic interpolation */
double cuberp(double values[4], double x);


// https://en.wikipedia.org/wiki/Inverse_distance_weighting
/** Spatial interpolation based on several known values at arbitrary locations. */
template <typename T_Pts, typename T_Data> class IDWInterpolator {
public:
    typedef std::pair<T_Pts, T_Data> DataPoint;

    IDWInterpolator(int p = 1, double radius = 1000000)
            : _p(p), _radius(radius) {}

    void setP(int p) { _p = p; }

    void setData(const std::vector<DataPoint> data) {
        _data = std::vector<DataPoint>(data);
    }

    void addData(T_Pts pt, T_Data data) { _data.emplace_back(pt, data); }

    T_Data getData(const T_Pts pt) const {
        T_Data sum; // TODO = T_Data::zero();
        double wSum = 0;

        for (const DataPoint &dp : _data) {
            auto length = T_Pts::length(pt, dp.first);

            if (length < std::numeric_limits<decltype(length)>::epsilon()) {
                return dp.second;
            } else {
                double weight = length < _radius ? powi(length, -_p) : 0;

                wSum = wSum + weight;
                sum = sum + dp.second * weight;
            }
        }

        if (wSum < std::numeric_limits<double>::epsilon()) {
            // Soit il n'y a pas de points, soit on en est trop loin. Dans les
            // deux cas, on retourne 0.
            return sum;
        }

        return sum * (1 / wSum);
    }

private:
    int _p;
    double _radius;
    std::vector<DataPoint> _data;
};
} // namespace world

#endif // WORLD_INTERPOLATION_H
