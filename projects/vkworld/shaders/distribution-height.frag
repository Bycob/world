#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "worldlib"

layout(location = 0) in vec2 fragCoord;
layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform DistributionParameters {
    vec4 height;
	vec4 dheight;
	float hmin;
	float hmax;
	float dhmin;
	float dhmax;
	float threshold;
	float slopeFactor;
};

layout(binding = 2) uniform PerlinParams {
	NoiseParameters uNoiseParams;
};

layout(binding = 3) uniform sampler2D random;
layout(binding = 4) uniform sampler2D inputHeight;
// layout(binding = 3) uniform sampler2D inputDHeight;

#define PI 3.14

void main() {
	vec2 uv = fragCoord;
	float h = texture(inputHeight, uv).r;
	// TODO Compute gradient of the texture
	float dh = atan(texture(inputHeight, uv).r * slopeFactor) * 2. / PI;
	
	float r1 = ramp(height.x, height.y, height.z, height.w, hmin, hmax, h);
	float r2 = ramp(dheight.x, dheight.y, dheight.z, dheight.w, dhmin, dhmax, dh);
	float r = r1 * r2;
	NoiseParameters noiseParams = uNoiseParams;
	float x = perlin(noiseParams, uv.x, uv.y, 0);
	
	fragColor = vec4(smoothstep(r + threshold, r - threshold, x));
}