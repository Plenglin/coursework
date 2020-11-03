#version 450
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img;

uniform vec4 color;
uniform ivec2 center;
uniform int radius;

const ivec2 offset = center - radius;


void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy) + offset;
    ivec2 r_dist = pixel_coords - center;
    if (dot(r_dist, r_dist) > radius * radius) return;

    float norm_dist = 1 - length(r_dist) / radius;

    vec4 existing = imageLoad(img, pixel_coords);
    float a = 0.8 * norm_dist * norm_dist;
    vec4 c_out = mix(existing, color, vec4(a, a, a, 0));

    imageStore(img, pixel_coords, c_out);
}