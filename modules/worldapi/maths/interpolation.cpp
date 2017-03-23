#include "interpolation.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace maths {
    double interpolate(double x1, double y1, double x2, double y2, double x) {
        //Interpoler entre deux points identiques donne une horizontale
        if (x1 == x2) return y1;
        //Au del� des bornes on consid�re que l'on ne bouge plus.
        if (x >= x2) return y2;
        if (x < x1) return y1;

        //On se place entre 0 et 1
        double xFunc = (x - x1) / (x2 - x1);
        //double yFunc = 6 * pow(xFunc, 5) - 15 * pow(xFunc, 4) + 10 * pow(xFunc, 3);
        double yFunc = (1 - cos(xFunc * M_PI)) * 0.5;

        //On se replace sur l'intervalle d'�tude
        return yFunc * (y2 - y1) + y1;
    }

    //TODO enlever la duplication de code

    double interpolateLinear(double x1, double y1, double x2, double y2, double x) {
        //Interpoler entre deux points identiques donne une horizontale
        if (x1 == x2) return y1;
        //Au del� des bornes on consid�re que l'on ne bouge plus.
        if (x >= x2) return y2;
        if (x < x1) return y1;

        //On se place entre 0 et 1
        double xFunc = (x - x1) / (x2 - x1);
        double yFunc = xFunc;

        //On se replace sur l'intervalle d'�tude
        return yFunc * (y2 - y1) + y1;
    }
}
