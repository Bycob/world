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

float rand(vec2 v) {
    return fract(sin(dot(v + vec2(-8.5123, 23.2156), vec2(12.9898, 59.233))) * 4758.5453123);
}


float rand3(vec3 v) {
    return fract(sin(dot(v + vec3(-8.5123, 23.2156, 0.0), vec3(12.9898, 6.233, 0.84261))) * 47583.5453123);
}


float noise3(in vec3 uvx) {
    vec3 f = fract(uvx);
    vec3 i = floor(uvx);

    float a1 = rand3(i);
    float b1 = rand3(i + vec3(0.0, 1.0, 0.0));
    float c1 = rand3(i + vec3(1.0, 0.0, 0.0));
    float d1 = rand3(i + vec3(1.0, 1.0, 0.0));
    float a2 = rand3(i + vec3(0.0, 0.0, 1.0));
    float b2 = rand3(i + vec3(0.0, 1.0, 1.0));
    float c2 = rand3(i + vec3(1.0, 0.0, 1.0));
    float d2 = rand3(i + vec3(1.0, 1.0, 1.0));

    vec3 u = -2. * f * f * f + 3. * f * f;

    float a = mix(a1, a2, u.z);
    float b = mix(b1, b2, u.z);
    float c = mix(c1, c2, u.z);
    float d = mix(d1, d2, u.z);

    return mix(mix(a, b, u.y), mix(c, d, u.y), u.x);
}

float fbm3(in vec3 uvx) {
    float sum = 0.0;
    float amp = 0.0;
    float persistence = 0.7;
    vec3 stz = uvx;

    for (int i = 0; i < 8; ++i) {
        amp = amp / persistence + noise3(stz);
        sum = sum / persistence + 1.;
        stz *= 2.;
    }
    return amp / sum;
}

float fbm2(in vec2 uv) {
    return fbm3(vec3(uv, 0));
}

const vec3 SAND1 = vec3(163. / 255., 158. / 255., 131. / 255.);
const vec3 SAND2 = vec3(250. / 255., 236. / 255., 201. / 255.);

void main() {
    vec2 uv = fragCoord * 0.5 * size + offset;
    float dist = max(1.0, size.x * 0.1);

    float val = fbm2(uv) * 0.3 + rand(uv) * 0.7;
    val = (val - 0.5) / dist + 0.5;

    vec3 col = vec3(mix(SAND1, SAND2, val));

    // Output to screen
    fragColor = vec4(col,1.0);
}
