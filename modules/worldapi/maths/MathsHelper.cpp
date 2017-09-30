#include <math.h>

#include "MathsHelper.h"

namespace maths {
	double length(const arma::vec3 & vec1, const arma::vec3 & vec2) {
		double x = vec1[0] - vec2[0];
		double y = vec1[1] - vec2[1];
		double z = vec1[2] - vec2[2];
		return sqrt(x * x + y * y + z * z);
	}
}
