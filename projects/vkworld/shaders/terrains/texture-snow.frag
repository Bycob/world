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

#define PI 3.14

void main() {
    vec2 uv = fragCoord * size + offset;
    fragColor = vec4(0.95, 1.0, 1.0, 1);
}
