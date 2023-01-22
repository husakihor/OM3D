#version 450

#include "utils.glsl"

// fragment shader of the main lighting pass

layout(location = 0) out vec4 out_color;

layout(binding = 0) uniform sampler2D in_albedo;
layout(binding = 1) uniform sampler2D in_normal;
layout(binding = 2) uniform sampler2D in_depth;

layout(binding = 3) uniform Data {
    FrameData frame;
};

layout(binding = 4) buffer PointLights {
    PointLight point_lights[];
};

layout(binding = 5) uniform WindowData {
    Window window;
};

const vec3 ambient = vec3(0.0);

void main() {
    vec4 encoded_albedo = texelFetch(in_albedo, ivec2(gl_FragCoord.xy), 0);
    vec4 encoded_normal = texelFetch(in_normal, ivec2(gl_FragCoord.xy), 0);

#if defined(NORMAL_DISPLAY)
    out_color = vec4(encoded_normal.xyz, 1.0);
#elif defined(ALBEDO_DISPLAY)
    out_color = vec4(encoded_albedo.rgb, 1.0);
#else
    vec3 albedo = encoded_albedo.rgb;
    vec3 normal = encoded_normal.xyz;
    normal = normal * 2.0 - 1.0;

    float depth = texelFetch(in_depth, ivec2(gl_FragCoord.xy), 0).r;
    if (depth == 0.0) {
        discard;
    }

    vec2 uv = gl_FragCoord.xy / vec2(window.width, window.height);
    vec3 position = unproject(uv, depth, inverse(frame.camera.view_proj));

    vec3 acc = frame.sun_color * max(0.0, dot(frame.sun_dir, normal)) + ambient;

    for(uint i = 0; i < frame.point_light_count; ++i) {
        PointLight light = point_lights[i];
        const vec3 to_light = (light.position - position);
        const float dist = length(to_light);
        const vec3 light_vec = to_light / dist;

        const float NoL = dot(light_vec, normal);
        const float att = attenuation(dist, light.radius);
        if(NoL <= 0.0 || att <= 0.0f) {
            continue;
        }

        acc += light.color * (NoL * att);
    }

    out_color = vec4(albedo * acc, 1.0);
#endif
}

