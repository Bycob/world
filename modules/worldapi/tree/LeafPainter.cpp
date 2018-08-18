#include "LeafPainter.h"

#include "math/Bezier.h"

namespace world {

LeafPainter::LeafPainter() {}

void LeafPainter::paint(Image &image) {
    vec3d joinPos{0, 0.5, 0};
    vec3d endPos{0.8, 0.2, 0};

    vec3d curvAtJoin{0, 0.25, 0};
    vec3d curvAtEnd{0.2, -0.27, 0};

    BezierCurve stem{joinPos, endPos, curvAtJoin, curvAtEnd};

    vec3d startPos = stem.getPointAt(0.25);

    vec3d weight1 = stem.getDerivativeAt(0.25).normalize() * 0.3;
    vec3d weight2 = vec3d{} - weight1;

    BezierCurve side1{startPos, weight1, endPos, curvAtEnd};

    BezierCurve side2{startPos, weight2, endPos, curvAtEnd};

    // fill between the curves

    // draw stem
}

} // namespace world
