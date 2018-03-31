#ifndef WORLD_WORLDTYPES_H
#define WORLD_WORLDTYPES_H

#include <cstdint>

#include <optional/optional.hpp>

// TODO should we use the standard library here ?

namespace world {

    typedef int8_t s8;
    typedef int16_t s16;
    typedef int32_t s32;
    typedef int64_t s64;

    typedef uint8_t u8;
    typedef uint16_t u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef float f32;
    typedef double f64;

	// Optionals (TODO restrict)
	template <class T> using optional = std::experimental::optional<T>;
	//template <class T> using optional<T&> = std::experimental::optional<T&>;
	using std::experimental::nullopt;
	using std::experimental::make_optional;
}

#endif //WORLD_WORLDTYPES_H
