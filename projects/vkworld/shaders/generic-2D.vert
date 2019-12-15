#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertCoord;
layout(location = 1) in vec3 vertNorm;
layout(location = 2) in vec2 vertUV;

layout(location = 0) out vec3 fragCoord;
layout(location = 1) out vec3 fragNorm;
layout(location = 2) out vec2 fragUV;

void main() {
    fragCoord = vertCoord;
    fragNorm = vertNorm;
    fragUV = vertUV;
    // Reverse image
    gl_Position = vec4(vec3(vertCoord.x, -vertCoord.y, vertCoord.z), 1.0);
}
