#version 450 
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_input;
layout(rgba32f, binding = 1) uniform image2D img_output;
layout(rgba32f, binding = 2) uniform image2D img_wall;

bool is_wall(ivec2 coords) {
    return imageLoad(img_wall, coords).a > 0;
}

vec4 bilinearInterp(vec2 coords) {
	vec2 weight = fract(coords);
	ivec2 coordsfloor = ivec2(coords);

    ivec2 pbl = coordsfloor;
    ivec2 pbr = coordsfloor + ivec2(1, 0);
    ivec2 ptl = coordsfloor + ivec2(0, 1);
    ivec2 ptr = coordsfloor + ivec2(1, 1);

	vec4 bl = is_wall(pbl) ? vec4(0.5, 0.5, 0.5, 0) : imageLoad(img_input, pbl);
	vec4 br = is_wall(pbr) ? vec4(0.5, 0.5, 0.5, 0) : imageLoad(img_input, pbr);
	vec4 tl = is_wall(ptl) ? vec4(0.5, 0.5, 0.5, 0) : imageLoad(img_input, ptl);
	vec4 tr = is_wall(ptr) ? vec4(0.5, 0.5, 0.5, 0) : imageLoad(img_input, ptr);

	vec2 to_bl = weight;
	vec2 to_br = vec2(-(1-weight.x), weight.y);
	vec2 to_tl = vec2(weight.x, -(1-weight.y));
	vec2 to_tr = vec2(-(1-weight.x), -(1-weight.y));

	vec4 bot = mix(bl, br, weight.x);
	vec4 top = mix(tl, tr, weight.x);
	vec4 newpos = mix(bot, top, weight.y);

	newpos.xyz = normalize(newpos.rgb - vec3(0.5,0.5,0.5)) * newpos.a;
	return newpos;
}

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    if (is_wall(pixel_coords)) return;

    ivec2 pl = pixel_coords + ivec2(-1,0);
    ivec2 pu = pixel_coords + ivec2(0,-1);
    ivec2 pr = pixel_coords + ivec2(1,0);
    ivec2 pd = pixel_coords + ivec2(0,1);

	bool wl = is_wall(pl);
    bool wu = is_wall(pu);
    bool wr = is_wall(pr);
    bool wd = is_wall(pd);

	vec4 col=imageLoad(img_input, pixel_coords);

	vec4 l,u,r,d;//left up right down
	l=u=r=d=col;
	if(pixel_coords.x>0)	l=imageLoad(img_input, pl);
	if(pixel_coords.y>0)	d=imageLoad(img_input, pu);
	if(pixel_coords.x<1920-1)	r=imageLoad(img_input, pr);
	if(pixel_coords.y<1080-1)	u=imageLoad(img_input, pd);

	vec4 va;
	va.xyz = normalize(col.rgb - vec3(0.5,0.5,0.5)) * col.a;
	va.a = col.a;
	vec3 vr = normalize(r.rgb - vec3(0.5,0.5,0.5)) * r.a;
	vec3 vl = normalize(l.rgb - vec3(0.5,0.5,0.5)) * l.a;
	vec3 vu = normalize(u.rgb - vec3(0.5,0.5,0.5)) * u.a;
	vec3 vd = normalize(d.rgb - vec3(0.5,0.5,0.5)) * d.a;

	//Advection
	float p = length(va.xy);
	if(pixel_coords.x>0 && pixel_coords.y>0 &&
		pixel_coords.x<1200-1 && pixel_coords.y<720-1) {
		vec2 pos = pixel_coords - p * va.xy;
		va = bilinearInterp(pos);
	}


	//Diffusion
	float alpha = 10.;
	float beta = 4. + alpha;
	va.xyz = (vl+vr+vd+vu+alpha*va.xyz)/beta;
	va.a = (l.a+r.a+d.a+u.a+alpha*va.a)/beta;

	//Pressure
	float hrdx = 0.25;
	float dpl = l.a - va.a;
	float dpu = u.a - va.a;
	float dpr = r.a - va.a;
	float dpd = d.a - va.a;
	va.xy -= hrdx*vec2(dpr-dpl, dpu-dpd);

	col.rgb = normalize(va.xyz)/2. + vec3(0.5,0.5,0.5);
	col.a = va.a;

	imageStore(img_output, pixel_coords, col);
}