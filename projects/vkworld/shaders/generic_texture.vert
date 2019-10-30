#version 450
#extension GL_ARB_separate_shader_objects : enable

vec3 positions[4] = {
    vec3(0, 0, 0),
    vec3(1, 0, 0),
    vec3(1, 1, 0),
    vec3(0, 1, 0)
};

layout(location = 0) out vec3 fragVert;

void main() {
    fragVert = positions[gl_VertexIndex];
    gl_Position = vec4(positions[gl_VertexIndex], 1.0);
}
