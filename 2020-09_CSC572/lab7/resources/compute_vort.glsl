#version 450 
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_input;
layout(rgba32f, binding = 1) uniform image2D img_output;
layout(rgba32f, binding = 2) uniform image2D img_wall;

bool is_wall(ivec2 coords) {
    return imageLoad(img_wall, coords).a > 0;
}

vec4 getPixel(ivec2 pixel_coords) {
    if (is_wall(pixel_coords)) {
        return vec4(0, 0, 0, 0);
    }
	vec4 col=imageLoad(img_input, pixel_coords);
	vec4 va;

	va.xyz = normalize(col.rgb - vec3(0.5,0.5,0.5)) * col.a;
	va.a = col.a;

	return va;
}

float curl(ivec2 coords) {
    ivec2 pd = coords + ivec2(0,-1);
    ivec2 pu = coords + ivec2(0,1);
    ivec2 pr = coords + ivec2(1,0);
    ivec2 pl = coords + ivec2(-1,0);

    // Walls do not move
	vec4 d = is_wall(pd) ? vec4(0, 0, 0, 0): getPixel(pd);
	vec4 u = is_wall(pu) ? vec4(0, 0, 0, 0): getPixel(pu);
	vec4 r = is_wall(pr) ? vec4(0, 0, 0, 0): getPixel(pr);
	vec4 l = is_wall(pl) ? vec4(0, 0, 0, 0): getPixel(pl);

	return u.x - d.x + l.y - r.y;
}

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    if (is_wall(pixel_coords)) return;

    ivec2 dims = imageSize(img_input);
	vec4 va = getPixel(pixel_coords);

	ivec2 pd = pixel_coords + ivec2(0, -1);
    ivec2 pu = pixel_coords + ivec2(0, 1);
    ivec2 pr = pixel_coords + ivec2(1, 0);
    ivec2 pl = pixel_coords + ivec2(-1, 0);

	//Vorticity
	if (pixel_coords.x>0 && pixel_coords.x<dims.x-1 && pixel_coords.y>0 && pixel_coords.y<dims.y-1) {
		float vort = 5.0, dt = 0.01;
		float dird = is_wall(pd) ? 0 : abs(curl(pd));
		float diru = is_wall(pu) ? 0 : abs(curl(pu));
		float dirr = is_wall(pr) ? 0 : abs(curl(pr));
		float dirl = is_wall(pl) ? 0 : abs(curl(pl));
		vec2 dir;
		dir.x = dird - diru;
		dir.y = dirr - dirl;
		dir = vort / (length(dir) + 1e-5f) * dir;
		va.xy = va.xy + dt * curl(pixel_coords) * dir;
	}

    vec4 col;
    col.rgb = normalize(va.xyz)/2. + vec3(0.5,0.5,0.5);
	col.a = va.a;

	imageStore(img_output, pixel_coords, col);
}