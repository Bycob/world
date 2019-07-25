#ifndef WORLD_RANDOMHELPER_H
#define WORLD_RANDOMHELPER_H

#include "world/core/WorldConfig.h"

#include <random>

#include "MathsHelper.h"

namespace world {

/** Scale input value to a random factor. Scale factors near 1 are chosen
 * more often. */
template <class RNG>
inline double randScale(RNG &rng, double value, double e = 1.05) {
    static std::normal_distribution<double> distribution;
    return value * pow(e, distribution(rng));
}

/** Create an exponential distribution with the given median value. */
inline std::exponential_distribution<double> exponentialDistribFromMedian(
    double median) {
    return std::exponential_distribution<double>(M_LN2 / median);
}
} // namespace world

#endif // WORLD_RANDOMHELPER_H
