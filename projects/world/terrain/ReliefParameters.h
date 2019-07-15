#pragma once

#include "world/core/WorldConfig.h"

#include "world/core/Parameters.h"

// DATA

#include "world/data/repartition_elevation001"

namespace world {

typedef Parameter<double> ElevationParam;
typedef Parameter<double, double> AltDiffParam;

struct ReliefMapParams : Params<double> {

    static ElevationParam CustomWorldElevation(double seaLevel = 0.5) {
        // 3 peaks : underwater (-2000, 0), overwater (1900, 2400), montains
        // (2400, 4000)
        // <!> Take into account the fact that this parameter represents only
        // half of the final altitude. ie 2000 elevation -> 0 avg altitude if
        // diff == 0

        static const double data[] = {__REPARTITION_ELEVATION001_DATA};

        ElevationParam ret;
        ret.setFunction([]() {
            auto distrib = std::uniform_real_distribution<double>(0, 1);
            return 1;
            data[static_cast<int>(distrib(rng()) *
                                  (__REPARTITION_ELEVATION001_SIZE - 1))];
        });
        return ret;
    }

    /**
     * @param seaLevel level of the sea, relatively to the "elevation"
     * parameter. When elevation > seaLevel, it means that at this point
     * we are over sea level. */
    static AltDiffParam CustomWorldDifferential(double seaLevel = 0.5) {
        // Under sea level (-2000, 0) : (0, 0.5) -> (0, 0.01)
        // Over sea level (0, 1000) : (0, 0.05) -> (0, 0.5)
        // Montains layer : grow a peak around 1
        AltDiffParam ret;
        ret.setFunction([seaLevel](const double &elevation) {
            // Magic numbers explication (see tanh curve for reference) :
            double a, b;
            if (elevation <= seaLevel) {
                // Under sea level : x {-4, 0} -> {-4, -2}
                double d = (seaLevel - elevation) / seaLevel;
                a = -4 + d * 0;
                b = 0 - d * 2;
            } else {
                // Over sea level : x {-3, -1} -> {1, 3}
                double d = (elevation - seaLevel) / (1 - seaLevel);
                a = -3 + d * 4;
                b = -1 + d * 4;
            }
            double x = std::uniform_real_distribution<double>(a, b)(
                Params<double>::rng());

            return tanh(x) * 0.5 + 0.5;
        });
        return ret;
    }
};
} // namespace world
