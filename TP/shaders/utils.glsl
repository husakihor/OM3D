#include "structs.glsl"

const float PI = 3.1415926535897932384626433832795028841971693993751058209749445923078164062;

float sqr(float x) {
    return x * x;
}

float saturate(float x) {
    return min(1.0, max(0.0, x));
}

vec2 saturate(vec2 x) {
    return min(vec2(1.0), max(vec2(0.0), x));
}

vec3 saturate(vec3 x) {
    return min(vec3(1.0), max(vec3(0.0), x));
}

vec4 saturate(vec4 x) {
    return min(vec4(1.0), max(vec4(0.0), x));
}

float luminance(vec3 rgb) {
    return dot(rgb, vec3(0.2126, 0.7152, 0.0722));
}

float attenuation(float distance, float radius) {
    const float x = min(distance, radius);
    return sqr(1.0 - sqr(sqr(x / radius))) / (sqr(x) + 1.0);
}

float attenuation(float distance, float radius, float falloff) {
    return attenuation(distance * falloff, radius * falloff);
}

float sRGB_to_linear(float x) {
    if(x <= 0.04045) {
        return x / 12.92;
    }
    return pow((x + 0.055) / 1.055, 2.4);
}

float linear_to_sRGB(float x) {
    if(x <= 0.0031308) {
        return x * 12.92;
    }
    return 1.055 * pow(x, 1.0 / 2.4) - 0.055;
}

vec3 sRGB_to_linear(vec3 v) {
    return vec3(sRGB_to_linear(v.r), sRGB_to_linear(v.g), sRGB_to_linear(v.b));
}

vec3 linear_to_sRGB(vec3 v) {
    return vec3(linear_to_sRGB(v.r), linear_to_sRGB(v.g), linear_to_sRGB(v.b));
}

vec3 unpack_normal_map(vec2 normal) {
    normal = normal * 2.0 - vec2(1.0);
    return vec3(normal, 1.0 - sqrt(dot(normal, normal)));
}

vec3 unproject(vec2 uv, float depth, mat4 inv_viewproj) {
    const vec3 ndc = vec3(uv * 2.0 - vec2(1.0), depth);
    const vec4 p = inv_viewproj * vec4(ndc, 1.0);
    return p.xyz / p.w;
}

float atan2(float y, float x)
{
    bool s = (abs(x) > abs(y));
    return mix(PI / 2.0 - atan(x, y), atan(y, x), s);
}