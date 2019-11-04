#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragCoord;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 fragColor;

void main() {
    fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}
