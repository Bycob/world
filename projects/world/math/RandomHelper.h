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

/** Rounds randomly the given value according to its distance with the
 * nearest integer. ie 3.75 will have 75% chance to be rounded to 4,
 * whereas 3.25 will have 75% being rounded to 3. */
template <class RNG> inline int randRound(RNG &rng, double value) {
    static std::uniform_real_distribution<double> distribution;
    return static_cast<int>(value - floor(value) > distrib(rng) ? floor(value)
                                                                : ceil(value));
}
} // namespace world

#endif // WORLD_RANDOMHELPER_H
