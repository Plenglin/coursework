#version 450
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img;

uniform vec4 color;
uniform ivec2 center;
uniform int radius;

const ivec2 offset = center - radius;


void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    ivec2 delta = pixel_coords - center;

    if (delta > radius * radius) return;

    imageStore(img_output, pixel_coords, color);
}