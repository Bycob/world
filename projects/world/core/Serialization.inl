#include <world/math/Vector.h>

namespace world {
// Vector 3

template <> inline void write<vec3d>(const vec3d &s, WorldFile &wf) {
    wf.addFloating("x", s.x);
    wf.addFloating("y", s.y);
    wf.addFloating("z", s.z);
}

template <> inline void read<vec3d>(const WorldFile &wf, vec3d &s) {
    wf.readDoubleOpt("x", s.x);
    wf.readDoubleOpt("y", s.y);
    wf.readDoubleOpt("z", s.z);
}

template <> inline void write<vec3f>(const vec3f &s, WorldFile &wf) {
    wf.addFloating("x", s.x);
    wf.addFloating("y", s.y);
    wf.addFloating("z", s.z);
}

template <> inline void read<vec3f>(const WorldFile &wf, vec3f &s) {
    wf.readFloatOpt("x", s.x);
    wf.readFloatOpt("y", s.y);
    wf.readFloatOpt("z", s.z);
}

template <> inline void write<vec3i>(const vec3i &s, WorldFile &wf) {
    wf.addInt("x", s.x);
    wf.addInt("y", s.y);
    wf.addInt("z", s.z);
}

template <> inline void read<vec3i>(const WorldFile &wf, vec3i &s) {
    wf.readIntOpt("x", s.x);
    wf.readIntOpt("y", s.y);
    wf.readIntOpt("z", s.z);
}

/*
template <>
inline void write<vec3u>(const vec3u &s, WorldFile & wf) {
    wf.addInt("x", s.x);
    wf.addInt("y", s.y);
    wf.addInt("z", s.z);
}

template <>
inline void read<vec3u>(const WorldFile &wf, vec3u &s) {
    wf.readIntOpt("x", s.x);
    wf.readIntOpt("y", s.y);
    wf.readIntOpt("z", s.z);
}
 */

// Vector 2

template <> inline void write<vec2d>(const vec2d &s, WorldFile &wf) {
    wf.addFloating("x", s.x);
    wf.addFloating("y", s.y);
}

template <> inline void read<vec2d>(const WorldFile &wf, vec2d &s) {
    wf.readDoubleOpt("x", s.x);
    wf.readDoubleOpt("y", s.y);
}

template <> inline void write<vec2f>(const vec2f &s, WorldFile &wf) {
    wf.addFloating("x", s.x);
    wf.addFloating("y", s.y);
}

template <> inline void read<vec2f>(const WorldFile &wf, vec2f &s) {
    wf.readFloatOpt("x", s.x);
    wf.readFloatOpt("y", s.y);
}

template <> inline void write<vec2i>(const vec2i &s, WorldFile &wf) {
    wf.addInt("x", s.x);
    wf.addInt("y", s.y);
}

template <> inline void read<vec2i>(const WorldFile &wf, vec2i &s) {
    wf.readIntOpt("x", s.x);
    wf.readIntOpt("y", s.y);
}

/*
template <>
inline void write<vec2u>(const vec2u &s, WorldFile & wf) {
    wf.addInt("x", s.x);
    wf.addInt("y", s.y);
}
 */
} // namespace world