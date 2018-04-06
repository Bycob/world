#pragma once

#include "core/WorldConfig.h"

#include <iostream>
#include <algorithm>
#include <limits>
#include <cmath>

// Math defines on MSVC
#ifdef _MSC_VER
#include <math.h>
#endif

#include <armadillo/armadillo>

namespace world {

	template <typename T>
	class WORLDAPI_EXPORT Function {
	public:
		virtual T operator() (T x) const = 0;
	};

	template <typename T>
	constexpr inline auto abs(T v) -> T {
		return v >= 0 ? v : - v;
	}

	template <typename T1, typename T2>
	constexpr inline auto max(T1 v1, T2 v2) -> decltype(v1 + v2) {
		return v1 > v2 ? v1 : v2;
	}

	template <typename T1, typename T2>
	constexpr inline auto min(T1 v1, T2 v2) -> decltype(v1 + v2) {
		return v1 < v2 ? v1 : v2;
	}

	template <typename T1, typename T2, typename T3>
	constexpr inline auto clamp(T1 value, T2 min, T3 max) -> decltype(value + min + max) {
		return value < min ? min : (value > max ? max : value);
	};

	/** @returns n modulo r, compris entre 0 et r - 1 */
	template <typename T>
	inline T mod(T n, T r) {
		T m = n % r;
		return m < 0 ? m + r : m;
	}

	/** @returns b^e, using fast exponential algorithm.
	 * @param e The signed integer exponent*/
	template <typename T>
	inline T powi(T b, int e) {
		const int m = e % 2;
		if (e < 0) {
			return powi(1 / b, - e);
		}
		else if (e == 0) {
			return 1;
		}
		else if (m == 1) {
			return b * powi(b, e - 1);
		}
		else {
			const T t = powi(b, e / 2);
			return t * t;
		}
	}
}