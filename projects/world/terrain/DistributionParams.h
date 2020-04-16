#ifndef VKWORLD_DISTRIBUTION_PARAMS_H
#define VKWORLD_DISTRIBUTION_PARAMS_H

#include "world/core/WorldConfig.h"

#include "world/core/WorldFile.h"

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

template <>
inline void write<DistributionParams>(const DistributionParams &pi,
                                      WorldFile &wf) {
    wf.addFloat("ha", pi.ha);
    wf.addFloat("hb", pi.hb);
    wf.addFloat("hc", pi.hc);
    wf.addFloat("hd", pi.hd);
    wf.addFloat("dha", pi.dha);
    wf.addFloat("dhb", pi.dhb);
    wf.addFloat("dhc", pi.dhc);
    wf.addFloat("dhd", pi.dhd);
    wf.addFloat("hmin", pi.hmin);
    wf.addFloat("hmax", pi.hmax);
    wf.addFloat("dhmin", pi.dhmin);
    wf.addFloat("dhmax", pi.dhmax);
    wf.addFloat("threshold", pi.threshold);
    wf.addFloat("slopeFactor", pi.slopeFactor);
}

template <>
inline void read<DistributionParams>(const WorldFile &wf,
                                     DistributionParams &pi) {
    wf.readFloatOpt("ha", pi.ha);
    wf.readFloatOpt("hb", pi.hb);
    wf.readFloatOpt("hc", pi.hc);
    wf.readFloatOpt("hd", pi.hd);
    wf.readFloatOpt("dha", pi.dha);
    wf.readFloatOpt("dhb", pi.dhb);
    wf.readFloatOpt("dhc", pi.dhc);
    wf.readFloatOpt("dhd", pi.dhd);
    wf.readFloatOpt("hmin", pi.hmin);
    wf.readFloatOpt("hmax", pi.hmax);
    wf.readFloatOpt("dhmin", pi.dhmin);
    wf.readFloatOpt("dhmax", pi.dhmax);
    wf.readFloatOpt("threshold", pi.threshold);
    wf.readFloatOpt("slopeFactor", pi.slopeFactor);
}

} // namespace world

#endif // VKWORLD_DISTRIBUTION_PARAMS_H
