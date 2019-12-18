#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "worldlib"

layout(location = 0) in vec2 fragCoord;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform Metadata {
    vec2 offset;
    vec2 size;
    uint texCount;
};
/*
TODO
layout(binding = 1) uniform sampler2DArray distributions;
layout(binding = 2) uniform sampler2DArray textures;
*/
#define MAX_TEX_COUNT 8
layout(binding = 1) uniform sampler2D distributions[MAX_TEX_COUNT];
layout(binding = 2) uniform sampler2D textures[MAX_TEX_COUNT];

#define PI 3.14

void main() {
    vec2 uv = fragUV;
    vec2 texUV = uv * size + offset;
    vec4 newColor = vec4(0);

    for (int i = 0; i < texCount; ++i) {
        // newColor = applyAlpha(newColor, texture(textures, vec3(texUV, i)) * vec4(vec3(1), texture(distributions, vec3(uv, i)).r));
        newColor = applyAlpha(newColor, texture(textures[i], texUV) * vec4(vec3(1), texture(distributions[i], uv).r));
    }

    fragColor = newColor;
}
