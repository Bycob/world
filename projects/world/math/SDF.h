#ifndef WORLD_SDF_H
#define WORLD_SDF_H

#include "world/core/WorldConfig.h"

namespace world {

/** Compute distance of point p to segment delimited by a and b. */
template <template <typename> class TVector, typename TType>
TType segmentSDF(const TVector<TType> &A, const TVector<TType> &B,
                 const TVector<TType> &P) {
    TType ll = (B - A).squaredNorm();
    TType h = world::min(1, world::max(0, (P - A).dotProduct(B - A) / ll));
    return (P - A - (B - A) * h).norm();
}

} // namespace world

#endif // WORLD_SDF_H
