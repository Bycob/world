#include "interpolation.h"

#include <math.h>

namespace maths {
	double CosineInterpolation::operator()(double x) const {
		return (1 - cos(x * M_PI)) * 0.5;
	}

	double LinearInterpolation::operator()(double x) const {
		return x;
	}

	const CosineInterpolation cosineInterpolation;
	const LinearInterpolation linearInterpolation;

	double interpolate(double x1, double y1, double x2, double y2, double x, const Function<double> * func) {
		//Interpoler entre deux points identiques donne une horizontale
		if (x1 == x2) return y1;
		//Au delà des bornes on considère que l'on ne bouge plus.
		if (x >= x2) return y2;
		if (x < x1) return y1;

		//On se place entre 0 et 1
		double xFunc = (x - x1) / (x2 - x1);
		//double yFunc = 6 * pow(xFunc, 5) - 15 * pow(xFunc, 4) + 10 * pow(xFunc, 3);
		double yFunc = (*func)(xFunc);

		//On se replace sur l'intervalle d'étude
		return yFunc * (y2 - y1) + y1;
	}

    double interpolateCosine(double x1, double y1, double x2, double y2, double x) {
		return interpolate(x1, y1, x2, y2, x, &cosineInterpolation);
    }

    double interpolateLinear(double x1, double y1, double x2, double y2, double x) {
		return interpolate(x1, y1, x2, y2, x, &linearInterpolation);
    }
}
