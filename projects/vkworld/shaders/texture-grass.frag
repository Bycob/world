#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_GOOGLE_include_directive : enable

#include "worldlib"

layout(location = 0) in vec2 fragCoord;

layout(location = 0) out vec4 fragColor;

layout(binding = 0) uniform TextureData {
    vec2 offset;
    vec2 size;
};

layout(binding = 1) uniform sampler2D randTex;

#define BLADES_SPACING 0.004
#define JITTER_MAX 0.004
// depends on size of grass blades in pixels
#define LOOKUP_DIST 10

#define HASHSCALE1 .1031
#define HASHSCALE3 vec3(.1031, .1030, .0973)

#define PI 3.14


float hash12(vec2 p)
{
	/*vec3 p3  = fract(vec3(p.xyx) * HASHSCALE1);
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);*/
    return texture(randTex, p).r;
}

///  3 out, 2 in...
vec3 hash32(vec2 p)
{
	/*vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yxz+19.19);
    return fract((p3.xxy+p3.yzz)*p3.zyx);*/
    return texture(randTex, p).gbr;
}

/// 2 out, 2 in...
vec2 hash22(vec2 p)
{
	/*vec3 p3 = fract(vec3(p.xyx) * HASHSCALE3);
    p3 += dot(p3, p3.yzx+19.19);
    return fract((p3.xx+p3.yz)*p3.zy);*/
    return texture(randTex, p).bg;
}

vec3 getGrassColor(float x) {
    vec3 a = vec3(0.2, 0.4, 0.3);
    vec3 b = vec3(0.3, 0.5, 0.2);
    vec3 c = vec3(0.2, 0.4, 0.2);
    vec3 d = vec3(0.66, 0.77, 0.33);
    vec3 col = a + b * cos(2. * PI * (c * x + d));
    return col;
}

float getGrassBlade(in vec2 position, in vec2 grassPos, out vec4 color) {
	// between {-1, -1, -1} and {1, 1, 1}
    vec3 grassVector3 = hash32(grassPos * 12.3512) * 2.0 - vec3(1);
    // keep grass z between 0 and 0.4
    grassVector3.z = grassVector3.z * 0.2 + 0.2;
    vec2 grassVector2 = normalize(grassVector3.xy);

    float grassLength = hash12(grassPos * 1.0235) * 0.01 + 0.012;

    // take coordinates in grass blade frame
    vec2 gv = position - grassPos;
    float gx = dot(grassVector2, gv);
    float gy = dot(vec2(-grassVector2.y, grassVector2.x), gv);
    float gxn = gx / grassLength;

    // TODO make gy depends to gx
    if (gxn >= 0.0 && gxn <= 1.0 && abs(gy) <= 0.0008 * (1 - gxn * gxn)) {
        vec3 thisGrassColor = getGrassColor(hash12(grassPos * 2.6316));
        color = vec4(thisGrassColor * (0.2 + 0.8 * gxn), 1.0);
    	return grassVector3.z * gxn;
    }
    else {
        color = vec4(0., 0., 0., 1.);
        return -1.0;
    }
}

float getPoint(in vec2 position, out vec4 color) {
   	int xcount = int(1. / BLADES_SPACING);
    int ycount = int(1. / BLADES_SPACING);
    int ox = int(position.x * float(xcount));
    int oy = int(position.y * float(ycount));

    float maxz = 0.0;

    for (int i = -LOOKUP_DIST; i < LOOKUP_DIST; ++i) {
        for (int j = -LOOKUP_DIST; j < LOOKUP_DIST; ++j) {
            vec2 upos = vec2(ox + i, oy + j);
            vec2 grassPos = (upos * BLADES_SPACING + hash22(upos) * JITTER_MAX);

            vec4 tempColor;
            float z = getGrassBlade(position, grassPos, tempColor);

            if (z > maxz) {
                maxz = z;
                color = tempColor;
            }
        }
    }
    if (maxz == 0.0) {
        color = vec4(0.);
    }

    return maxz;
}

void main() {
    vec2 uv = fragCoord * size + offset;
    uv /= 3;
    getPoint(uv, fragColor);
}
