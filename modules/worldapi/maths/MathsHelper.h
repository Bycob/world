#pragma once

#include <worldapi/worldapidef.h>

#include <iostream>
#include <algorithm>
#include <limits>
#include <armadillo/armadillo>

#include "Vector.h"

namespace world {

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

	template <typename T1, typename T2, typename T3>
	inline auto clamp(T1 value, T2 min, T3 max) -> decltype(value + min + max) {
		return value < min ? min : (value > max ? max : value);
	};

	/** @returns n modulo r, compris entre 0 et r - 1 */
	template <typename T> inline T mod(T n, T r) {
		T m = n % r;
		return m < 0 ? m + r : m;
	}

	template <typename T>
	inline T powi(T b, int e) {
		const int m = e % 2;
		if (e == 0) {
			return 1;
		}
		else if (m == 1) {
			return b * powi(b, e - 1);
		}
		else if (m == -1) {
			return powi(b, e + 1) / b;
		}
		else {
			const T t = powi(b, e / 2);
			return t * t;
		}
	}
	
	double WORLDAPI_EXPORT length(const arma::vec3 & vec1, const arma::vec3 & vec2);
}