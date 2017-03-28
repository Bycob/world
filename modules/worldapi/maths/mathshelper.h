#pragma once

#include <worldapi/worldapidef.h>

#include <iostream>
#include <algorithm>
#include <limits>
#include <armadillo/armadillo>

namespace maths {

	//TODO déplacer vec3 à un endroit plus apropprié
	template <typename T>
	struct vec3 {
		T x;
		T y;
		T z;

		vec3() : x(0), y(0), z(0) {}
		vec3(T x, T y, T z) : x(x), y(y), z(z) {}

		vec3<T> operator+(const vec3<T> & rhs) const;
		vec3<T> operator-(const vec3<T> & rhs) const;
		vec3<T> operator*(T rhs) const;
		vec3<T> operator/(T rhs) const;

		vec3<T> normalize() const;
		T norm() const;

		vec3<T> crossProduct(const vec3<T> &rhs) const;
		T dotProduct(const vec3<T> &rhs) const;
	};

	template<typename T> inline vec3<T> vec3<T>::operator+(const vec3<T> & rhs) const {
		return vec3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
	}

	template<typename T> inline vec3<T> vec3<T>::operator-(const vec3<T> & rhs) const {
		return vec3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
	}

	template<typename T> inline vec3<T> vec3<T>::operator*(T rhs) const {
		return vec3(this->x * rhs, this->y * rhs, this->z * rhs);
	}

	template<typename T> inline vec3<T> vec3<T>::operator/(T rhs) const {
		return vec3(this->x / rhs, this->y / rhs, this->z / rhs);
	}

	template<typename T> inline T vec3<T>::norm() const {
		return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	template<typename T> inline vec3<T> vec3<T>::normalize() const {
		T norm = this->norm();
		if (norm <= std::numeric_limits<T>::epsilon()) return *this;
		return vec3(this->x / norm, this->y / norm, this->z / norm);
	}

	template<typename T> inline vec3<T> vec3<T>::crossProduct(const vec3<T> & rhs) const {
		return vec3(
			this->y * rhs.z - rhs.y * this->z,
			this->z * rhs.x - rhs.z * this->x,
			this->x * rhs.y - rhs.x * this->y
		);
	}

	template<typename T> inline T vec3<T>::dotProduct(const vec3<T> & rhs) const {
		return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
	}

	template<typename T> inline T getAngle(const vec3<T> & vec1, const vec3<T> & vec2) {
		T n1 = vec1.norm();
		T n2 = vec2.norm();
		T n12 = n1 * n2;
		if (n12 < std::numeric_limits<T>::epsilon()) return 0;
		T c = vec1.dotProduct(vec2) / n12;
		if (c > 1) c = 1; if (c < -1) c = -1;
		return acos(c);
	}

	template struct WORLDAPI_EXPORT vec3<double>;
	typedef vec3<double> vec3d;

	std::ostream& operator<<(std::ostream& os, const vec3d& vec);

	template <typename T>
	class WORLDAPI_EXPORT Function {
		virtual T operator() (T x) const = 0;
	};

	template <typename T> inline T max(T v1, T v2) {
		return v1 > v2 ? v1 : v2;
	}

	template <typename T> inline T min(T v1, T v2) {
		return v1 < v2 ? v1 : v2;
	}

	// Renvoie n modulo r
	template <typename T> inline T mod(T n, T r) {
		return n >= 0 ? n % r : (n % r) + r;
	}
	
	double WORLDAPI_EXPORT length(const arma::vec3 & vec1, const arma::vec3 & vec2);
	double WORLDAPI_EXPORT length(const vec3d & vec1, const vec3d & vec2);
}