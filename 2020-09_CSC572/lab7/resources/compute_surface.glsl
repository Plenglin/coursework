#version 450
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_flow;
layout(rgba32f, binding = 1) uniform image2D img_output;
layout(rgba32f, binding = 2) uniform image2D img_wall;
layout(rgba32f, binding = 3) uniform image2D img_input;

bool is_wall(ivec2 coords) {
    return imageLoad(img_wall, coords).a > 0;
}

vec4 getPixel(ivec2 pixel_coords) {
    if (is_wall(pixel_coords)) {
        return vec4(0, 0, 0, 0);
    }
	vec4 col = imageLoad(img_flow, pixel_coords);
	vec4 va;

	va.xyz = normalize(col.rgb - vec3(0.5,0.5,0.5)) * col.a;
	va.a = col.a;

	return va;
}

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    if (is_wall(pixel_coords)) {
       	imageStore(img_output, pixel_coords, vec4(1, 1, 0, 1));
        return;
    }

    ivec2 dims = imageSize(img_input);
	vec4 va = getPixel(pixel_coords);

    ivec2 surrounding[] = {ivec2(1, 0), ivec2(0, 1), ivec2(-1, 0), ivec2(0, -1)};

    // Compute the influences
    vec4 this_color = imageLoad(img_input, pixel_coords);
    vec4 color_delta = vec4(0, 0, 0, 0);
    for (uint i = 0; i < 4; i++) {
        ivec2 dir = surrounding[i];
        vec2 flow = getPixel(pixel_coords + dir).xy;
        float outflow = dot(dir, flow);  // Color is biased by how much it's pushing into this
        vec4 this_delta = imageLoad(img_input, pixel_coords + dir) - this_color;
        color_delta -= outflow * this_delta;
        //color_delta.xy = flow;
    }

    //this_color += 0.05 * color_delta;
    this_color = imageLoad(img_flow, pixel_coords);
    this_color = clamp(this_color, vec4(0, 0, 0, 0), vec4(1, 1, 1, 1));
    this_color.a = 1;

	imageStore(img_output, pixel_coords, this_color);
}