#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragCoord;
layout(location = 1) in vec3 fragNorm;
layout(location = 2) in vec2 fragUV;

layout(location = 0) out vec4 fragColor;

// between 0 and 1
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

void main() {
    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = fragCoord;

    float val = abs(fbm(uv * vec2(1., 0.1)) - .5);
    vec3 col = vec3(val * val) * 12. * vec3(0.35, 0.25, 0.1);

    // Output to screen
    fragColor = vec4(col,1.0);
}
