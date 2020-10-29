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
	vec4 u = getPixel(coords+ivec2(0,1));
	vec4 d = getPixel(coords+ivec2(0,-1));
	vec4 l = getPixel(coords+ivec2(-1,0));
	vec4 r = getPixel(coords+ivec2(1,0));
	return u.x-d.x + l.y-r.y;
}

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    if (is_wall(pixel_coords)) return;

	vec4 col;
	vec4 va = getPixel(pixel_coords);

	ivec2 pd = pixel_coords+ivec2(0,-1);
    ivec2 pu = pixel_coords+ivec2(0,1);
    ivec2 pr = pixel_coords+ivec2(1,0);
    ivec2 pl = pixel_coords+ivec2(-1,0);

	//Vorticity
	if (pixel_coords.x>0 && pixel_coords.x<1200-1
		&& pixel_coords.y>0 && pixel_coords.y<720-1) {
		float vort = 10.0, dt = 0.011;
		vec2 dir;
		float dird = abs(curl(pixel_coords+ivec2(0,-1)));
		float diru = abs(curl(pixel_coords+ivec2(0,1)));
		float dirr = abs(curl(pixel_coords+ivec2(1,0)));
		float dirl = abs(curl(pixel_coords+ivec2(-1,0)));
		dir.x = dird-diru;
		dir.y = dirr-dirl;
		dir = vort/(length(dir)+1e-5f)*dir;
		va.xy = va.xy+dt*curl(pixel_coords)*dir;
	}

	col.rgb = normalize(va.xyz)/2. + vec3(0.5,0.5,0.5);
	col.a = va.a;

	imageStore(img_output, pixel_coords, col);
	}