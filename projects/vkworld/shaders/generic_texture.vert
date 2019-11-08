#version 450
#extension GL_ARB_separate_shader_objects : enable

vec3 positions[6] = {
    vec3(-1, -1, 0),
    vec3(1, -1, 0),
    vec3(1, 1, 0),
    vec3(-1, -1, 0),
    vec3(1, 1, 0),
    vec3(-1, 1, 0)
};

layout(location = 0) out vec2 fragCoord;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragUV;

void main() {
    fragCoord = positions[gl_VertexIndex].xy;
    fragNorm = vec3(0, 0, 1);
    fragUV = (positions[gl_VertexIndex].xy + 1) / 2;
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
}
