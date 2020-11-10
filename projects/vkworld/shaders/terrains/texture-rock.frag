#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "../worldlib"

layout(location = 0) in vec2 fragCoord;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform TextureData {
    vec2 offset;
	vec2 size;
};

layout(binding = 1) uniform sampler2D randTex;

layout(binding = 2) uniform Color {
    vec3 rockColor;
};

#define PI 3.14

// between 0 and 1
float rand(vec2 v) {
    return fract(sin(dot(v + vec2(-8.5123, 23.2156), vec2(12.9898, 59.233))) * 4758.5453123);
}

vec2 rand2(vec2 v) {
    return vec2(
    rand(v * vec2(4562.223, 1232.465) + vec2(1122.0, 8325.0)),
    rand(v * vec2(2312.843, 8621.456) + vec2(5133.2, 2462.7)));
}

float noise(in vec2 uv) {
    vec2 f = fract(uv);
    vec2 i = floor(uv);

    float a = rand(i);
    float b = rand(i + vec2(0.0, 1.0));
    float c = rand(i + vec2(1.0, 0.0));
    float d = rand(i + vec2(1.0, 1.0));

    vec2 u = -2. * f * f * f + 3. * f * f;
    return mix(mix(a, b, u.y), mix(c, d, u.y), u.x);
}

float fbm2(in vec2 uv) {
    float sum = 0.0;
    float amp = 0.0;
    float persistence = 0.8;
    vec2 st = uv;

    for (int i = 0; i < 6; ++i) {
        amp = amp / persistence + noise(st);
        sum = sum / persistence + 1.;
        st *= 2.;
    }
    return amp / sum;
}

const int N = 8;

float voronoi(in vec2 uv) {
    // split in squares
    const float space = 0.4;
    vec2 rf = vec2(1.0, 0.5);
    vec2 rs = vec2(0.5, 1.6);

    // take n sample in each square
    vec2 uvi = vec2(floor(uv / space - 0.5));
    vec2 p1 = uvi * space;
    vec2 p2 = (uvi + vec2(0, 1)) * space;
    vec2 p3 = (uvi + vec2(1, 0)) * space;
    vec2 p4 = (uvi + vec2(1, 1)) * space;
    float m = 10000.0;

    for (int i = 0; i < N; i++) {
        p1 = (uvi + rand2(p1 * rf + rs)) * space;
        m = min(m, distance(p1, uv));
        p2 = (uvi + vec2(0, 1) + rand2(p2 * rf + rs)) * space;
        m = min(m, distance(p2, uv));
        p3 = (uvi + vec2(1, 0) + rand2(p3 * rf + rs)) * space;
        m = min(m, distance(p3, uv));
        p4 = (uvi + vec2(1, 1) + rand2(p4 * rf + rs)) * space;
        m = min(m, distance(p4, uv));
    }

    return 1. - pow(m, 0.5) / space * sqrt(float(N)) * 0.2;
}

void main() {
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord * 0.5 * size + offset;
    vec2 a = vec2(voronoi(uv + vec2(4.2, 2.6)), voronoi(uv + vec2(8.2, 5.6)));
    vec2 b = vec2(fbm2(uv), fbm2(uv + vec2(5.)));

    vec3 col = rockColor * (voronoi(uv + (b - 0.5) * 0.4));
    col *= fbm2(uv);

    fragColor = vec4(col,1.0);
}
