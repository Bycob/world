#pragma once

#include "world/core/WorldConfig.h"

#include <random>

#include "world/math/Vector.h"
#include "world/assets/Image.h"

namespace world {

class WORLDAPI_EXPORT Lightning {
public:
    Lightning();

    void generateLightning(Image &img, const vec2d &from);

private:
    std::mt19937_64 _rng;

    double _minStep = 3;
    double _maxStep = 20;
    u32 _stepCount = 100;
    double _angleVar = M_PI / 12.0;
    double _subdivideChance = 0.05;
    int _maxSubdivisions = 10;
};

} // namespace world
