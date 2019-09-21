// From BynaryNoiseLib
float rand(vec2 v) {
    return fract(sin(dot(v + vec2(-8.5123, 23.2156), vec2(12.9898, 59.233))) * 4758.5453123);
}

float noise(in vec2 uv) {
    vec2 f = fract(uv);
    vec2 i = floor(uv);

    float a = rand(i);
    float b = rand(i + vec2(0.0, 1.0));
    float c = rand(i + vec2(1.0, 0.0));
    float d = rand(i + vec2(1.0, 1.0));

    vec2 u = -2. * f * f * f + 3. * f * f;
    return mix(mix(a, b, u.y), mix(c, d, u.y), u.x);
}

float fbm(in vec2 uv) {
    float sum = 0.0;
    float amp = 0.0;
    float persistence = 0.7;
    vec2 st = uv;

    for (int i = 0; i < 8; ++i) {
        amp = amp / persistence + noise(st);
        sum = sum / persistence + 1.;
        st *= 2.;
    }
    return amp / sum;
}

// From VkWorld
float ramp(float a, float b, float c, float d, float lowb, float highb, float x) {
    float ya = (x - a) / (b - a);
    float yc = (d - x) / (d - c);
    float yr = min(ya, yc);
    return clamp(yr, lowb, highb);
}

// ----------------

// Uniforms
struct LayerParams {
    vec4 height;
    vec4 dheight;
    float hmin;
    float hmax;
    float dhmin;
    float dhmax;
    float threshold;
    float slopeFactor;
};

#define MAX_TEXTURE_COUNT 16

layout(binding = 0) uniform Global {
    uint textureCount;
    float tileScale;
};

layout(binding = 1) uniform Layers {
    LayerParams layerParams[MAX_TEXTURE_COUNT];
};

layout(binding = 2) uniform sampler2DArray textures;

layout(location = 0) in vec3 fragVert;
layout(location = 1) in vec3 fragNorm;

layout(location = 0) out vec4 finalColor;

float getDensity(int id, float h, float dh) {
    LayerParams p = layerParams[id];
    float r1 = ramp(p.height.x, p.height.y, p.height.z, p.height.w, p.hmin, p.hmax, h);
    float r2 = ramp(p.dheight.x, p.dheight.y, p.dheight.z, p.dheight.w, p.dhmin, p.dhmax, dh);
    float r = r1 * r2;
    float x = fbm(fragVert.xy);
    return smoothstep(r + p.threshold, r - p.threshold, x);
}

void main() {
    float h = fragVert.z;
    float dh = atan(length(fragNorm.xy) / fragNorm.z);
    vec4 color = vec4(0);

    for (int i = 0; i < textureCount; ++i) {
        float alpha = getDensity(i, h, dh);
        vec4 pixel = texture(textures, vec3(fragVert.xy * tileScale, i));
        color = vec4(pixel.xyz * alpha + color.xyz * (1 - alpha), 1);
    }

    finalColor = color;
}