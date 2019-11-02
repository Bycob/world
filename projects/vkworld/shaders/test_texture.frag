#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fragCoord;

layout(location = 0) out vec4 fragColor;

float rand3(vec3 v) {
    return fract(sin(dot(v + vec3(-8.5123, 23.2156, 0.0), vec3(12.9898, 6.233, 0.84261))) * 47583.5453123);
}

float noise3(in vec3 uvx) {
    vec3 f = fract(uvx);
    vec3 i = floor(uvx);

    float a1 = rand3(i);
    float b1 = rand3(i + vec3(0.0, 1.0, 0.0));
    float c1 = rand3(i + vec3(1.0, 0.0, 0.0));
    float d1 = rand3(i + vec3(1.0, 1.0, 0.0));
    float a2 = rand3(i + vec3(0.0, 0.0, 1.0));
    float b2 = rand3(i + vec3(0.0, 1.0, 1.0));
    float c2 = rand3(i + vec3(1.0, 0.0, 1.0));
    float d2 = rand3(i + vec3(1.0, 1.0, 1.0));

    vec3 u = -2. * f * f * f + 3. * f * f;

    float a = mix(a1, a2, u.z);
    float b = mix(b1, b2, u.z);
    float c = mix(c1, c2, u.z);
    float d = mix(d1, d2, u.z);

    return mix(mix(a, b, u.y), mix(c, d, u.y), u.x);
}


float fbm3(in vec3 uvx) {
    float sum = 0.0;
    float amp = 0.0;
    float persistence = 0.7;
    vec3 stz = uvx;

    for (int i = 0; i < 8; ++i) {
        amp = amp / persistence + noise3(stz);
        sum = sum / persistence + 1.;
        stz *= 2.;
    }
    return amp / sum;
}

float fbm2(in vec2 uv) {
    return fbm3(vec3(uv, 0));
}

float fbmsin(in vec2 uv) {
    vec2 p = sin(uv);
    vec2 r = p;
    return fbm2(uv + r);
}

void mainOsc(in vec2 uv, out vec4 fragColor) {
    // Time varying pixel color
    vec3 col = vec3(fbmsin(uv), fbmsin(uv + vec2(4.0)), fbmsin(uv + vec2(8.0)));

    // Output to screen
    fragColor = vec4(col,1.0);
}

void main() {
    mainOsc(fragCoord, fragColor);
}
