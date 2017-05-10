#pragma once

#include <worldapi/worldapidef.h>

#include <iostream>
#include <algorithm>
#include <limits>
#include <armadillo/armadillo>

#include "Vector.h"

namespace maths {

	template <typename T>
	class WORLDAPI_EXPORT Function {
	public:
		virtual T operator() (T x) const = 0;
	};

	template <typename T1, typename T2> inline auto max(T1 v1, T2 v2) -> decltype(v1 + v2) {
		return v1 > v2 ? v1 : v2;
	}

	template <typename T1, typename T2> inline auto min(T1 v1, T2 v2) -> decltype(v1 + v2) {
		return v1 < v2 ? v1 : v2;
	}

	// Renvoie n modulo r
	template <typename T> inline T mod(T n, T r) {
		return n >= 0 ? n % r : (n % r) + r;
	}
	
	double WORLDAPI_EXPORT length(const arma::vec3 & vec1, const arma::vec3 & vec2);
}