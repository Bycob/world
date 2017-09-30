#pragma once

#include <irrlicht.h>

#include <worldapi/maths/MathsHelper.h>

template <typename T> inline irr::core::vector3df toIrrlicht(const maths::vec3<T> & vec) {
	return irr::core::vector3df(vec.x, vec.y, vec.z);
}

template <typename T> inline irr::core::vector2df toIrrlicht(const maths::vec2<T> & vec) {
	return irr::core::vector2df(vec.x, vec.y);
}