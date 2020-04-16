#include <world/math/Vector.h>
#include <world/math/Perlin.h>
#include <world/core/TileSystem.h>

namespace world {
// Vector 3

template <> inline void write<vec3d>(const vec3d &s, WorldFile &wf) {
    wf.addDouble("x", s.x);
    wf.addDouble("y", s.y);
    wf.addDouble("z", s.z);
}

template <> inline void read<vec3d>(const WorldFile &wf, vec3d &s) {
    wf.readDoubleOpt("x", s.x);
    wf.readDoubleOpt("y", s.y);
    wf.readDoubleOpt("z", s.z);
}

template <> inline void write<vec3f>(const vec3f &s, WorldFile &wf) {
    wf.addFloat("x", s.x);
    wf.addFloat("y", s.y);
    wf.addFloat("z", s.z);
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

template <> inline void write<vec3u>(const vec3u &s, WorldFile &wf) {
    wf.addInt("x", s.x);
    wf.addInt("y", s.y);
    wf.addInt("z", s.z);
}

template <> inline void read<vec3u>(const WorldFile &wf, vec3u &s) {
    wf.readUintOpt("x", s.x);
    wf.readUintOpt("y", s.y);
    wf.readUintOpt("z", s.z);
}

// Vector 2

template <> inline void write<vec2d>(const vec2d &s, WorldFile &wf) {
    wf.addDouble("x", s.x);
    wf.addDouble("y", s.y);
}

template <> inline void read<vec2d>(const WorldFile &wf, vec2d &s) {
    wf.readDoubleOpt("x", s.x);
    wf.readDoubleOpt("y", s.y);
}

template <> inline void write<vec2f>(const vec2f &s, WorldFile &wf) {
    wf.addDouble("x", s.x);
    wf.addDouble("y", s.y);
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

template <> inline void write<vec2u>(const vec2u &s, WorldFile &wf) {
    wf.addInt("x", s.x);
    wf.addInt("y", s.y);
}

// TileSystem

template <> inline void write<TileSystem>(const TileSystem &ts, WorldFile &wf) {
    wf.addInt("maxLod", ts._maxLod);
    wf.addInt("factor", ts._factor);
    wf.addStruct("bufferRes", ts._bufferRes);
    wf.addStruct("baseSize", ts._baseSize);
}

template <> inline void read<TileSystem>(const WorldFile &wf, TileSystem &ts) {
    wf.readIntOpt("maxLod", ts._maxLod);
    wf.readIntOpt("factor", ts._factor);
    wf.readStruct("bufferRes", ts._bufferRes);
    wf.readStruct("baseSize", ts._baseSize);
}

// Perlin info

template <> inline void write<PerlinInfo>(const PerlinInfo &pi, WorldFile &wf) {
    wf.addInt("octaves", pi.octaves);
    wf.addDouble("persistence", pi.persistence);
    wf.addBool("repeatable", pi.repeatable);
    wf.addInt("reference", pi.reference);
    wf.addDouble("frequency", pi.frequency);
    wf.addInt("offsetX", pi.offsetX);
    wf.addInt("offsetY", pi.offsetY);
}

template <> inline void read<PerlinInfo>(const WorldFile &wf, PerlinInfo &pi) {
    wf.readIntOpt("octaves", pi.octaves);
    wf.readDoubleOpt("persistence", pi.persistence);
    wf.readBoolOpt("repeatable", pi.repeatable);
    wf.readIntOpt("reference", pi.reference);
    wf.readDoubleOpt("frequency", pi.frequency);
    wf.readIntOpt("offsetX", pi.offsetX);
    wf.readIntOpt("offsetY", pi.offsetY);
}

} // namespace world