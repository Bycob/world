#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragCoord;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D random;

vec3 grassColor = vec3(0.48, 0.96, 0.34);

void main() {
    vec3 baseColor = fragUV.x > 0 ? grassColor : grassColor * 0.7;

    // dashes on grass
    vec2 texCoords = abs(fragUV);
    texCoords.x *= 0.2;
    texCoords.y *= 0.01;

    float randFactor = texture(random, texCoords).r * 0.4 + 0.6;

    fragColor = vec4(baseColor * randFactor, 1.0);
}
