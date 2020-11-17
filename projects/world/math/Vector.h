#pragma once

#include "world/core/WorldConfig.h"

#include <algorithm>
#include <limits>
#include <iostream>

#include "MathsHelper.h"

namespace world {

template <typename T> struct vec3;
template <typename T> struct vec2;

template <typename T> struct vec3 {
    T x;
    T y;
    T z;

    vec3() : x(0), y(0), z(0) {}
    vec3(T val) : x(val), y(val), z(val) {}
    vec3(T x, T y, T z) : x(x), y(y), z(z) {}

    template <typename R> operator vec3<R>() const;
    template <typename R> explicit operator vec2<R>() const;

    vec3<T> operator-() const;

    template <typename R>
    auto operator*(R rhs) const -> vec3<decltype(x * rhs)>;
    template <typename R>
    auto operator/(R rhs) const -> vec3<decltype(x / rhs)>;

    template <typename R>
    auto operator+(const vec3<R> &rhs) const -> vec3<decltype(x + rhs.x)>;
    template <typename R>
    auto operator-(const vec3<R> &rhs) const -> vec3<decltype(x - rhs.x)>;
    template <typename R>
    auto operator*(const vec3<R> &rhs) const -> vec3<decltype(x * rhs.x)>;
    template <typename R>
    auto operator/(const vec3<R> &rhs) const -> vec3<decltype(x / rhs.x)>;

    vec3<T> &operator*=(T rhs);
    vec3<T> &operator/=(T rhs);

    template <typename R> vec3<T> &operator+=(const vec3<R> &rhs);
    template <typename R> vec3<T> &operator-=(const vec3<R> &rhs);
    template <typename R> vec3<T> &operator*=(const vec3<R> &rhs);
    template <typename R> vec3<T> &operator/=(const vec3<R> &rhs);

    template <typename R> bool operator==(const vec3<R> &rhs) const;
    template <typename R> bool operator<(const vec3<R> &rhs) const;

    vec3<T> normalize() const;
    double norm() const;
    T squaredNorm() const;

    vec3<T> crossProduct(const vec3<T> &rhs) const;
    T dotProduct(const vec3<T> &rhs) const;

    T squaredLength(const vec3<T> &rhs) const;
    double length(const vec3<T> &rhs) const;

    vec3<T> floor() const;
    vec3<T> ceil() const;
    vec3<T> round() const;

    vec3<T> cos() const;
    vec3<T> sin() const;
    vec3<T> tan() const;

    vec3<T> clamp(const vec3<T> &min, const vec3<T> &max) const;

    static double length(const vec3<T> &vec1, const vec3<T> &vec2);

    static constexpr vec3<T> X() { return {1, 0, 0}; }
    static constexpr vec3<T> Y() { return {0, 1, 0}; }
    static constexpr vec3<T> Z() { return {0, 0, 1}; }
};

template <typename T> struct vec2 {
    T x;
    T y;

    vec2() : x(0), y(0) {}
    vec2(T val) : x(val), y(val) {}
    vec2(T x, T y) : x(x), y(y) {}

    template <typename R> operator vec2<R>() const;
    template <typename R> explicit operator vec3<R>() const;

    template <typename R>
    auto operator*(R rhs) const -> vec2<decltype(x * rhs)>;
    template <typename R>
    auto operator/(R rhs) const -> vec2<decltype(x / rhs)>;

    template <typename R>
    auto operator+(const vec2<R> &rhs) const -> vec2<decltype(x + rhs.x)>;
    template <typename R>
    auto operator-(const vec2<R> &rhs) const -> vec2<decltype(x - rhs.x)>;
    template <typename R>
    auto operator*(const vec2<R> &rhs) const -> vec2<decltype(x * rhs.x)>;
    template <typename R>
    auto operator/(const vec2<R> &rhs) const -> vec2<decltype(x / rhs.x)>;

    vec2<T> &operator*=(T rhs);
    vec2<T> &operator/=(T rhs);

    template <typename R> vec2<T> &operator+=(const vec2<R> &rhs);
    template <typename R> vec2<T> &operator-=(const vec2<R> &rhs);
    template <typename R> vec2<T> &operator*=(const vec2<R> &rhs);
    template <typename R> vec2<T> &operator/=(const vec2<R> &rhs);

    template <typename R> bool operator==(const vec2<R> &rhs) const;
    template <typename R> bool operator<(const vec2<R> &rhs) const;

    vec2<T> normalize() const;
    double norm() const;
    T squaredNorm() const;

    T dotProduct(const vec2<T> &rhs) const;

    T squaredLength(const vec2<T> &rhs) const;
    double length(const vec2<T> &rhs) const;

    vec2<T> floor() const;
    vec2<T> ceil() const;
    vec2<T> round() const;

    vec2<T> cos() const;
    vec2<T> sin() const;
    vec2<T> tan() const;

    vec2<T> clamp(const vec2<T> &min, const vec2<T> &max) const;

    static double length(const vec2<T> &vec1, const vec2<T> &vec2);

    static constexpr vec2<T> X() { return {1, 0}; }
    static constexpr vec2<T> Y() { return {0, 1}; }
};


// vec3

template <typename T>
template <typename R>
inline vec3<T>::operator vec3<R>() const {
    return vec3<R>(static_cast<R>(this->x), static_cast<R>(this->y),
                   static_cast<R>(this->z));
}

template <typename T>
template <typename R>
inline vec3<T>::operator vec2<R>() const {
    return vec2<R>(static_cast<R>(this->x), static_cast<R>(this->y));
}

template <typename T> inline vec3<T> vec3<T>::operator-() const {
    return vec3<T>{-this->x, -this->y, -this->z};
}

template <typename T>
template <typename R>
inline auto vec3<T>::operator*(R rhs) const -> vec3<decltype(x * rhs)> {
    return vec3<decltype(x * rhs)>(this->x * rhs, this->y * rhs, this->z * rhs);
}

template <typename T>
template <typename R>
inline auto vec3<T>::operator/(R rhs) const -> vec3<decltype(x / rhs)> {
    return vec3<decltype(x / rhs)>(this->x / rhs, this->y / rhs, this->z / rhs);
}

template <typename T>
template <typename R>
inline auto vec3<T>::operator+(const vec3<R> &rhs) const
    -> vec3<decltype(x + rhs.x)> {
    return vec3<decltype(x + rhs.x)>(this->x + rhs.x, this->y + rhs.y,
                                     this->z + rhs.z);
}

template <typename T>
template <typename R>
inline auto vec3<T>::operator-(const vec3<R> &rhs) const
    -> vec3<decltype(x - rhs.x)> {
    return vec3<decltype(x - rhs.x)>(this->x - rhs.x, this->y - rhs.y,
                                     this->z - rhs.z);
}

template <typename T>
template <typename R>
inline auto vec3<T>::operator*(const vec3<R> &rhs) const
    -> vec3<decltype(x * rhs.x)> {
    return vec3<decltype(x * rhs.x)>(this->x * rhs.x, this->y * rhs.y,
                                     this->z * rhs.z);
}

template <typename T>
template <typename R>
inline auto vec3<T>::operator/(const vec3<R> &rhs) const
    -> vec3<decltype(x / rhs.x)> {
    return vec3<decltype(x / rhs.x)>(this->x / rhs.x, this->y / rhs.y,
                                     this->z / rhs.z);
}

template <typename T> inline vec3<T> &vec3<T>::operator*=(T rhs) {
    x *= rhs;
    y *= rhs;
    z *= rhs;
    return *this;
}

template <typename T> inline vec3<T> &vec3<T>::operator/=(T rhs) {
    x /= rhs;
    y /= rhs;
    z /= rhs;
    return *this;
}

template <typename T>
template <typename R>
inline vec3<T> &vec3<T>::operator+=(const vec3<R> &rhs) {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

template <typename T>
template <typename R>
inline vec3<T> &vec3<T>::operator-=(const vec3<R> &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    z -= rhs.z;
    return *this;
}

template <typename T>
template <typename R>
inline vec3<T> &vec3<T>::operator*=(const vec3<R> &rhs) {
    x *= rhs.x;
    y *= rhs.y;
    z *= rhs.z;
    return *this;
}

template <typename T>
template <typename R>
inline vec3<T> &vec3<T>::operator/=(const vec3<R> &rhs) {
    x /= rhs.x;
    y /= rhs.y;
    z /= rhs.z;
    return *this;
}

template <typename T>
template <typename R>
inline bool vec3<T>::operator==(const vec3<R> &rhs) const {
    return x == rhs.x && y == rhs.y && z == rhs.z;
};

template <typename T>
template <typename R>
inline bool vec3<T>::operator<(const vec3<R> &rhs) const {
    return x < rhs.x ? true
                     : (x == rhs.x && y < rhs.y
                            ? true
                            : x == rhs.x && y == rhs.y && z < rhs.z);
}

template <typename T> inline double vec3<T>::norm() const {
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}

template <typename T> inline T vec3<T>::squaredNorm() const {
    return this->x * this->x + this->y * this->y + this->z * this->z;
}

template <typename T> inline vec3<T> vec3<T>::normalize() const {
    double norm = this->norm();
    if (norm <= std::numeric_limits<double>::epsilon())
        return *this;
    return vec3((T)(this->x / norm), (T)(this->y / norm), (T)(this->z / norm));
}

template <typename T>
inline vec3<T> vec3<T>::crossProduct(const vec3<T> &rhs) const {
    return vec3(this->y * rhs.z - rhs.y * this->z,
                this->z * rhs.x - rhs.z * this->x,
                this->x * rhs.y - rhs.x * this->y);
}

template <typename T> inline T vec3<T>::dotProduct(const vec3<T> &rhs) const {
    return this->x * rhs.x + this->y * rhs.y + this->z * rhs.z;
}

template <typename T>
inline T vec3<T>::squaredLength(const vec3<T> &rhs) const {
    T x = this->x - rhs.x;
    T y = this->y - rhs.y;
    T z = this->z - rhs.z;
    return x * x + y * y + z * z;
}

template <typename T> inline double vec3<T>::length(const vec3<T> &rhs) const {
    return sqrt(squaredLength(rhs));
}

template <typename T> inline vec3<T> vec3<T>::floor() const {
    return {T(::floor(x)), T(::floor(y)), T(::floor(z))};
}

template <typename T> inline vec3<T> vec3<T>::ceil() const {
    return {T(::ceil(x)), T(::ceil(y)), T(::ceil(z))};
}

template <typename T> inline vec3<T> vec3<T>::round() const {
    return {T(::round(x)), T(::round(y)), T(::round(z))};
}

template <typename T> inline vec3<T> vec3<T>::cos() const {
    return {static_cast<T>(::cos(x)), static_cast<T>(::cos(y)),
            static_cast<T>(::cos(z))};
}

template <typename T> inline vec3<T> vec3<T>::sin() const {
    return {static_cast<T>(::sin(x)), static_cast<T>(::sin(y)),
            static_cast<T>(::sin(z))};
}

template <typename T> inline vec3<T> vec3<T>::tan() const {
    return {static_cast<T>(::tan(x)), static_cast<T>(::tan(y)),
            static_cast<T>(::tan(z))};
}

template <typename T>
inline vec3<T> vec3<T>::clamp(const vec3<T> &min, const vec3<T> &max) const {
    return {world::clamp(x, min.x, max.x), world::clamp(y, min.y, max.y),
            world::clamp(z, min.z, max.z)};
}


template <typename T>
inline double vec3<T>::length(const vec3<T> &vec1, const vec3<T> &vec2) {
    return vec1.length(vec2);
}

template <typename T>
inline double getAngle(const vec3<T> &vec1, const vec3<T> &vec2) {
    double n1 = vec1.norm();
    double n2 = vec2.norm();
    double n12 = n1 * n2;
    if (n12 < std::numeric_limits<double>::epsilon())
        return 0;
    double c = vec1.dotProduct(vec2) / n12;
    if (c > 1)
        c = 1;
    if (c < -1)
        c = -1;
    return acos(c);
}

// vec3 - useful functions

template <typename T>
std::ostream &operator<<(std::ostream &os, const vec3<T> &vec) {
    os << " (" << vec.x << ", " << vec.y << ", " << vec.z << ") ";
    return os;
}


// vec2


template <typename T>
template <typename R>
inline vec2<T>::operator vec2<R>() const {
    return vec2<R>(static_cast<R>(this->x), static_cast<R>(this->y));
}

template <typename T>
template <typename R>
inline vec2<T>::operator vec3<R>() const {
    return vec3<R>(static_cast<R>(this->x), static_cast<R>(this->y),
                   static_cast<R>(0));
}

template <typename T>
template <typename R>
inline auto vec2<T>::operator*(R rhs) const -> vec2<decltype(x * rhs)> {
    return vec2<decltype(x * rhs)>(this->x * rhs, this->y * rhs);
}

template <typename T>
template <typename R>
inline auto vec2<T>::operator/(R rhs) const -> vec2<decltype(x / rhs)> {
    return vec2<decltype(x / rhs)>(this->x / rhs, this->y / rhs);
}

template <typename T>
template <typename R>
inline auto vec2<T>::operator+(const vec2<R> &rhs) const
    -> vec2<decltype(x + rhs.x)> {
    return vec2<decltype(x + rhs.x)>(this->x + rhs.x, this->y + rhs.y);
}

template <typename T>
template <typename R>
inline auto vec2<T>::operator-(const vec2<R> &rhs) const
    -> vec2<decltype(x - rhs.x)> {
    return vec2<decltype(x - rhs.x)>(this->x - rhs.x, this->y - rhs.y);
}

template <typename T>
template <typename R>
inline auto vec2<T>::operator*(const vec2<R> &rhs) const
    -> vec2<decltype(x * rhs.x)> {
    return vec2<decltype(x * rhs.x)>(this->x * rhs.x, this->y * rhs.y);
}

template <typename T>
template <typename R>
inline auto vec2<T>::operator/(const vec2<R> &rhs) const
    -> vec2<decltype(x / rhs.x)> {
    return vec2<decltype(x / rhs.x)>(this->x / rhs.x, this->y / rhs.y);
}

template <typename T> inline vec2<T> &vec2<T>::operator*=(T rhs) {
    x *= rhs;
    y *= rhs;
    return *this;
}

template <typename T> inline vec2<T> &vec2<T>::operator/=(T rhs) {
    x /= rhs;
    y /= rhs;
    return *this;
}

template <typename T>
template <typename R>
inline vec2<T> &vec2<T>::operator+=(const vec2<R> &rhs) {
    x += rhs.x;
    y += rhs.y;
    return *this;
}

template <typename T>
template <typename R>
inline vec2<T> &vec2<T>::operator-=(const vec2<R> &rhs) {
    x -= rhs.x;
    y -= rhs.y;
    return *this;
}

template <typename T>
template <typename R>
inline vec2<T> &vec2<T>::operator*=(const vec2<R> &rhs) {
    x *= rhs.x;
    y *= rhs.y;
    return *this;
}

template <typename T>
template <typename R>
inline vec2<T> &vec2<T>::operator/=(const vec2<R> &rhs) {
    x /= rhs.x;
    y /= rhs.y;
    return *this;
}

template <typename T>
template <typename R>
inline bool vec2<T>::operator<(const vec2<R> &rhs) const {
    return x < rhs.x ? true : x == rhs.x && y < rhs.y;
}

template <typename T>
template <typename R>
inline bool vec2<T>::operator==(const vec2<R> &rhs) const {
    return x == rhs.x && y == rhs.y;
}

template <typename T> inline double vec2<T>::norm() const {
    return sqrt(this->x * this->x + this->y * this->y);
}

template <typename T> inline T vec2<T>::squaredNorm() const {
    return this->x * this->x + this->y * this->y;
}

template <typename T> inline vec2<T> vec2<T>::normalize() const {
    double norm = this->norm();
    if (norm <= std::numeric_limits<double>::epsilon())
        return *this;
    return vec2<T>((T)(this->x / norm), (T)(this->y / norm));
}

template <typename T> inline T vec2<T>::dotProduct(const vec2<T> &rhs) const {
    return this->x * rhs.x + this->y * rhs.y;
}

template <typename T>
inline T vec2<T>::squaredLength(const vec2<T> &rhs) const {
    T x = this->x - rhs.x;
    T y = this->y - rhs.y;
    return x * x + y * y;
}

template <typename T> inline double vec2<T>::length(const vec2<T> &rhs) const {
    return sqrt(squaredLength(rhs));
}

template <typename T> inline vec2<T> vec2<T>::floor() const {
    return {T(::floor(x)), T(::floor(y))};
}

template <typename T> inline vec2<T> vec2<T>::ceil() const {
    return {T(::ceil(x)), T(::ceil(y))};
}

template <typename T> inline vec2<T> vec2<T>::round() const {
    return {T(::round(x)), T(::round(y))};
}

template <typename T> inline vec2<T> vec2<T>::cos() const {
    return {static_cast<T>(::cos(x)), static_cast<T>(::cos(y))};
}

template <typename T> inline vec2<T> vec2<T>::sin() const {
    return {static_cast<T>(::sin(x)), static_cast<T>(::sin(y))};
}

template <typename T> inline vec2<T> vec2<T>::tan() const {
    return {static_cast<T>(::tan(x)), static_cast<T>(::tan(y))};
}

template <typename T>
inline vec2<T> vec2<T>::clamp(const vec2<T> &min, const vec2<T> &max) const {
    return {world::clamp(x, min.x, max.x), world::clamp(y, min.y, max.y)};
}

template <typename T>
inline double vec2<T>::length(const vec2<T> &vec1, const vec2<T> &vec2) {
    return vec1.length(vec2);
}


// vec2 - useful functions

template <typename T>
std::ostream &operator<<(std::ostream &os, const vec2<T> vec) {
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
typedef vec3<float> vec3f;
typedef vec3<int> vec3i;
typedef vec3<unsigned int> vec3u;
typedef vec2<double> vec2d;
typedef vec2<float> vec2f;
typedef vec2<int> vec2i;
typedef vec2<unsigned int> vec2u;
} // namespace world