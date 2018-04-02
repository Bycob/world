#pragma once

#include <irrlicht.h>

#include <worldapi/assets/Color.h>
#include <worldapi/maths/MathsHelper.h>

template <typename T> inline irr::core::vector3df toIrrlicht(const world::vec3<T> & vec) {
	return irr::core::vector3df(
		static_cast<irr::f32>(vec.x), 
		static_cast<irr::f32>(vec.z), 
		static_cast<irr::f32>(vec.y));
}

template <typename T> inline irr::core::vector2df toIrrlicht(const world::vec2<T> & vec) {
	return irr::core::vector2df(
		static_cast<irr::f32>(vec.x), 
		static_cast<irr::f32>(vec.y));
}

inline irr::video::SColor toIrrColor(const world::Color4u &color) {
	return irr::video::SColor(color.getARGB());
}