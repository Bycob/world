#pragma once

#include "MathsHelper.h"
#include "Vector.h"

namespace world {

class BezierCurve {
public:
    vec3d _pts[4];

    BezierCurve(vec3d pt1, vec3d pt2, vec3d pt1Weight, vec3d pt2Weight)
            : _pts{pt1, pt1 + pt1Weight, pt2 + pt2Weight, pt2} {}

    /** Returns coordinates of the curve point for t equals to the
     * given value. t is the parameter of the curve.
     * Special case : t = 0 returns pt1, t = 1 returns pt2. */
    vec3d getPointAt(double t) const {
        double omt = 1 - t;

        double b1 = omt * omt * omt;
        double b2 = omt * omt * t;
        double b3 = omt * t * t;
        double b4 = t * t * t;

        return _pts[0] * b1 + _pts[1] * (b2 * 3) + _pts[2] * (b3 * 3) +
               _pts[3] * b4;
    }

    vec3d getDerivativeAt(double t) const {
        const vec3d pt12 = _pts[1] - _pts[0];
        const vec3d pt23 = _pts[2] - _pts[1];
        const vec3d pt34 = _pts[3] - _pts[2];

        double omt = 1 - t;

        double b1 = omt * omt;
        double b2 = omt * t;
        double b3 = t * t;

        return pt12 * (3 * b1) + pt23 * (6 * b2) + pt34 * (3 * b3);
    }
};
} // namespace world