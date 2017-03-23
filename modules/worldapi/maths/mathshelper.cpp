#include <math.h>

#include "mathshelper.h"

namespace maths {
	double length(const arma::vec3 & vec1, const arma::vec3 & vec2) {
		double x = vec1[0] - vec2[0];
		double y = vec1[1] - vec2[1];
		double z = vec1[2] - vec2[2];
		return sqrt(x * x + y * y + z * z);
	}
	
	double length(const vec3d & vec1, const vec3d & vec2) {
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
