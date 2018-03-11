#pragma once

#include <worldapi/worldapidef.h>

#include <algorithm>
#include <limits>
#include <iostream>

namespace maths {

	template <typename T>
	struct vec3 {
		T x;
		T y;
		T z;

		vec3() : x(0), y(0), z(0) {}
		vec3(T x, T y, T z) : x(x), y(y), z(z) {}

		template <typename R> operator vec3<R>() const;

		template<typename R> auto operator*(R rhs) const -> vec3<decltype(x * rhs)>;
		template<typename R> auto operator/(R rhs) const -> vec3<decltype(x / rhs)>;
		
		template <typename R> auto operator+(const vec3<R> & rhs) const -> vec3<decltype(x + rhs.x)>;
		template <typename R> auto operator-(const vec3<R> & rhs) const -> vec3<decltype(x - rhs.x)>;
		template<typename R> auto operator*(const vec3<R> & rhs) const -> vec3<decltype(x * rhs.x)>;
		template<typename R> auto operator/(const vec3<R> & rhs) const -> vec3<decltype(x / rhs.x)>;

		template <typename R> bool operator==(const vec3<R> &rhs) const;
		template<typename R> bool operator<(const vec3<R> & rhs) const;

		vec3<T> normalize() const;
		T norm() const;

		vec3<T> crossProduct(const vec3<T> &rhs) const;
		T dotProduct(const vec3<T> &rhs) const;

		T squaredLength(const vec3<T> &rhs) const;
		T length(const vec3<T> &rhs) const;

		static T length(const vec3<T> &vec1, const vec3<T> &vec2);
	};

	template <typename T>
	struct vec2 {
		T x;
		T y;

		vec2() : x(0), y(0) {}
		vec2(T x, T y) : x(x), y(y) {}

		vec2<T> operator*(T rhs) const;
		vec2<T> operator/(T rhs) const;

		vec2<T> operator+(const vec2<T> & rhs) const;
		vec2<T> operator-(const vec2<T> & rhs) const;

		template <typename R> bool operator<(const vec2<R> & rhs) const;

		vec2<T> normalize() const;
		T norm() const;

		T dotProduct(const vec2<T> &rhs) const;

		T squaredLength(const vec2<T> &rhs) const;
		T length(const vec2<T> &rhs) const;

		static T length(const vec2<T> &vec1, const vec2<T> &vec2);
	};


	// vec3

	template<typename T>
	template<typename R>
	inline vec3<T>::operator vec3<R>() const {
		return vec3<R>((R) this->x, (R) this->y, (R) this->z);
	}

	template<typename T>
	template<typename R>
	inline auto vec3<T>::operator*(R rhs) const -> vec3<decltype(x * rhs)> {
		return vec3(this->x * rhs, this->y * rhs, this->z * rhs);
	}

	template<typename T>
	template<typename R>
	inline auto vec3<T>::operator/(R rhs) const -> vec3<decltype(x / rhs)> {
		return vec3(this->x / rhs, this->y / rhs, this->z / rhs);
	}

	template<typename T>
	template<typename R>
	inline auto vec3<T>::operator+(const vec3<R> & rhs) const -> vec3<decltype(x + rhs.x)> {
		return vec3(this->x + rhs.x, this->y + rhs.y, this->z + rhs.z);
	}

	template<typename T>
	template<typename R>
	inline auto vec3<T>::operator-(const vec3<R> & rhs) const -> vec3<decltype(x - rhs.x)> {
		return vec3(this->x - rhs.x, this->y - rhs.y, this->z - rhs.z);
	}

	template<typename T>
	template<typename R>
	inline auto vec3<T>::operator*(const vec3<R> & rhs) const -> vec3<decltype(x * rhs.x)> {
		return vec3(this->x * rhs.x, this->y * rhs.y, this->z * rhs.z);
	}

	template<typename T>
	template<typename R>
	inline auto vec3<T>::operator/(const vec3<R> & rhs) const -> vec3<decltype(x / rhs.x)> {
		return vec3(this->x / rhs.x, this->y / rhs.y, this->z / rhs.z);
	}

	template <typename T>
	template <typename R>
	inline bool vec3<T>::operator==(const vec3<R> &rhs) const {
		return x == rhs.x && y == rhs.y && z == rhs.z;
	};

	template<typename T>
	template<typename R>
	inline bool vec3<T>::operator<(const vec3<R> & rhs) const {
		return x < rhs.x ? true :
			(x == rhs.x && y < rhs.y ? true :
				x == rhs.x && y == rhs.y && z < rhs.z);
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

	template<typename T> inline T vec3<T>::squaredLength(const vec3<T> & rhs) const {
		double x = this->x - rhs.x;
		double y = this->y - rhs.y;
		double z = this->z - rhs.z;
		return x * x + y * y + z * z;
	}

	template<typename T> inline T vec3<T>::length(const vec3<T> & rhs) const {
		return sqrt(squaredLength(rhs));
	}

	template<typename T> inline T vec3<T>::length(const vec3<T> & vec1, const vec3<T> & vec2) {
		return vec1.length(vec2);
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

	template <typename T> std::ostream& operator<<(std::ostream& os, const vec3<T>& vec) {
		os << " (" << vec.x << ", " << vec.y << ", " << vec.z << ") ";
		return os;
	}


	// vec2

	template<typename T> inline vec2<T> vec2<T>::operator+(const vec2<T> & rhs) const {
		return vec2(this->x + rhs.x, this->y + rhs.y);
	}

	template<typename T> inline vec2<T> vec2<T>::operator-(const vec2<T> & rhs) const {
		return vec2(this->x - rhs.x, this->y - rhs.y);
	}

	template<typename T> inline vec2<T> vec2<T>::operator*(T rhs) const {
		return vec2(this->x * rhs, this->y * rhs);
	}

	template<typename T> inline vec2<T> vec2<T>::operator/(T rhs) const {
		return vec2(this->x / rhs, this->y / rhs);
	}

	template<typename T>
	template<typename R> 
	inline bool vec2<T>::operator<(const vec2<R> & rhs) const {
		return x < rhs.x ? true : x == rhs.x && y < rhs.y;
	}

	template<typename T> inline T vec2<T>::norm() const {
		return sqrt(this->x * this->x + this->y * this->y);
	}

	template<typename T> inline vec2<T> vec2<T>::normalize() const {
		T norm = this->norm();
		if (norm <= std::numeric_limits<T>::epsilon()) return *this;
		return vec2(this->x / norm, this->y / norm);
	}

	template<typename T> inline T vec2<T>::dotProduct(const vec2<T> & rhs) const {
		return this->x * rhs.x + this->y * rhs.y;
	}

	template<typename T> inline T vec2<T>::squaredLength(const vec2<T> & rhs) const {
		double x = this->x - rhs.x;
		double y = this->y - rhs.y;
		return x * x + y * y;
	}

	template<typename T> inline T vec2<T>::length(const vec2<T> & rhs) const {
		return sqrt(squaredLength(rhs));
	}

	template<typename T> inline T vec2<T>::length(const vec2<T> & vec1, const vec2<T> & vec2) {
		return vec1.length(vec2);
	}

	template <typename T> std::ostream& operator<<(std::ostream& os, const vec2<T> vec) {
		os << " (" << vec.x << ", " << vec.y << ") ";
		return os;
	}


	// -----

	template struct WORLDAPI_EXPORT vec3<double>;
	template struct WORLDAPI_EXPORT vec3<float>;
	template struct WORLDAPI_EXPORT vec3<int>;
	template struct WORLDAPI_EXPORT vec2<double>;
	template struct WORLDAPI_EXPORT vec2<float>;
	template struct WORLDAPI_EXPORT vec2<int>;

	typedef vec3<double> vec3d;
	typedef vec3<int> vec3i;
	typedef vec2<double> vec2d;
	typedef vec2<int> vec2i;
}