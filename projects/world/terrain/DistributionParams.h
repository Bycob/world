#ifndef VKWORLD_DISTRIBUTION_PARAMS_H
#define VKWORLD_DISTRIBUTION_PARAMS_H

#include "world/core/WorldConfig.h"

namespace world {

struct WORLDAPI_EXPORT DistributionParams {
    float ha;
    float hb;
    float hc;
    float hd;
    float dha;
    float dhb;
    float dhc;
    float dhd;
    float hmin;
    float hmax;
    float dhmin;
    float dhmax;
    float threshold;
    float slopeFactor;
};

} // namespace world

#endif // VKWORLD_DISTRIBUTION_PARAMS_H
