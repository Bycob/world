#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragCoord;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform Metadata {
    vec3 grassColor;
};
/* layout(binding = 1) uniform sampler2D random;*/

void main() {
    fragColor = vec4(grassColor, 1.0);
}
