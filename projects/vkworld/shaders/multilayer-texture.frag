#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "worldlib"

layout(location = 0) in vec2 fragCoord;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform Metadata {
    uint texCount;
};
layout(binding = 1) uniform sampler2DArray textures;
layout(binding = 2) uniform sampler2DArray distributions;

#define PI 3.14

void main() {
    vec2 uv = fragCoord.xy;
    vec4 newColor = vec4(0);

    for (int i = 0; i < texCount; ++i) {
        newColor = applyAlpha(newColor,
            texture(textures, vec3(uv, i)) * vec4(vec3(1), texture(distributions, vec3(uv, i)).a));
    }

    fragColor = newColor;
}
