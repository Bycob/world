#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "worldlib"

layout(location = 0) in vec2 fragCoord;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform TextureData {
    vec2 offset;
    vec2 size;
};

#define PI 3.14

#define VORONOI_SPACE 0.001
#define JITTER_MAX 0.0012

#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)

float hash12(vec2 p)
{
	vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

/// 2 out, 2 in...
vec2 hash22(vec2 p)
{
	vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yzx+19.19);
    return fract((p3.xx+p3.yz)*p3.zy);

}

vec3 getSoilColor(float x) {
    vec3 a = vec3(0.13, 0.09, 0.06);
    vec3 b = vec3(0.08, 0.05, 0.02);
    vec3 c = vec3(0.3, 0.3, 0.2);
    vec3 d = vec3(0.8, 0.8, 0.5);
    vec3 col = a + b * cos(2. * PI * (c * x + d));
    return col;
}

vec4 getSoilAt(vec2 uv) {
   	int xcount = int(1. / VORONOI_SPACE);
    int ycount = int(1. / VORONOI_SPACE);
    int ox = int(uv.x * float(xcount));
    int oy = int(uv.y * float(ycount));

    vec2 pos;
   	float dist = 1000.0;

    for (int i = -2; i < 2; ++i) {
        for (int j = -2; j < 2; ++j) {
            vec2 upos = vec2(ox + i, oy + j);
            vec2 p = (upos * VORONOI_SPACE + hash22(upos) * JITTER_MAX);
            float d = distance(uv, p);

            if (d < dist) {
                pos = p;
                dist = d;
            }
        }
    }

    float val = 1.0 - clamp(dist / (VORONOI_SPACE * 3.), 0.0, 1.0);
    val = val * val;

    return vec4(getSoilColor(hash12(pos * 1000.0)) * val,1.0);
}

void main() {
    vec2 uv = fragCoord * 0.5 * size + offset;
    fragColor = getSoilAt(uv);
}
