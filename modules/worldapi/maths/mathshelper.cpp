#include <math.h>

#include "mathshelper.h"

namespace maths {

	double interpolate(double x1, double y1, double x2, double y2, double x) {
		//Interpoler entre deux points identiques donne une horizontale
		if (x1 == x2) return y1;
		//Au delà des bornes on considère que l'on ne bouge plus.
		if (x >= x2) return y2;
		if (x < x1) return y1;

		//On se place entre 0 et 1
		double xFunc = (x - x1) / (x2 - x1);
		//double yFunc = 6 * pow(xFunc, 5) - 15 * pow(xFunc, 4) + 10 * pow(xFunc, 3);
		double yFunc = (1 - cos(xFunc * 3.1415927)) * 0.5;

		//On se replace sur l'intervalle d'étude
		return yFunc * (y2 - y1) + y1;
	}

	//TODO enlever la duplication de code

	double interpolateLinear(double x1, double y1, double x2, double y2, double x) {
		//Interpoler entre deux points identiques donne une horizontale
		if (x1 == x2) return y1;
		//Au delà des bornes on considère que l'on ne bouge plus.
		if (x >= x2) return y2;
		if (x < x1) return y1;

		//On se place entre 0 et 1
		double xFunc = (x - x1) / (x2 - x1);
		double yFunc = xFunc;

		//On se replace sur l'intervalle d'étude
		return yFunc * (y2 - y1) + y1;
	}

	inline double length(const arma::vec3 & vec1, const arma::vec3 & vec2) {
		double x = vec1[0] - vec2[0];
		double y = vec1[1] - vec2[1];
		double z = vec1[2] - vec2[2];
		return sqrt(x * x + y * y + z * z);
	}
	
	inline double length(const vec3d & vec1, const vec3d & vec2) {
		double x = vec1.x - vec2.x;
		double y = vec1.y - vec2.y;
		double z = vec1.z - vec2.z;
		return sqrt(x * x + y * y + z * z);
	}

	std::ostream& operator<<(std::ostream& os, const vec3d& vec) {
		os << " (" << vec.x << ", " << vec.y << ", " << vec.z << ") ";
		return os;
	}
}