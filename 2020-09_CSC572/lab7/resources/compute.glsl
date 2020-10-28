#version 450 
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) uniform image2D img_input;
layout(rgba32f, binding = 1) uniform image2D img_output;
layout(rgba32f, binding = 2) uniform image2D img_wall;

vec4 bilinearInterp(vec2 coords) {
	vec2 weight = fract(coords);
	ivec2 coordsfloor = ivec2(coords);

	vec4 bl = imageLoad(img_input, coordsfloor);
	vec4 br = imageLoad(img_input, coordsfloor+ivec2(1,0));
	vec4 tl = imageLoad(img_input, coordsfloor+ivec2(0,1));
	vec4 tr = imageLoad(img_input, coordsfloor+ivec2(1,1));
	vec2 to_bl = weight;
	vec2 to_br = vec2(-(1-weight.x), weight.y);
	vec2 to_tl = vec2(weight.x, -(1-weight.y));
	vec2 to_tr = vec2(-(1-weight.x), -(1-weight.y));

	vec4 bot = mix(bl, br, weight.x);
	vec4 top = mix(tl, tr, weight.x);

	vec4 newpos = mix(bot, top, weight.y);
//	newpos.xy += (to_bl*(bl.a-newpos.a) + to_br*(br.a-newpos.a) + to_tl*(tl.a-newpos.a) + to_tr*(tr.a-newpos.a));
	newpos.xyz = normalize(newpos.rgb - vec3(0.5,0.5,0.5)) * newpos.a;
	return newpos;
}

void main() {
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
    if (imageLoad(img_wall, pixel_coords).a > 0) return;

	vec4 l,u,r,d;//left up right down				
	vec4 col=imageLoad(img_input, pixel_coords);
	vec4 va;
	vec3 vl,vr,vu,vd;

	l=u=r=d=col;
	if(pixel_coords.x>0)	l=imageLoad(img_input, pixel_coords + ivec2(-1,0));
	if(pixel_coords.y>0)	d=imageLoad(img_input, pixel_coords + ivec2(0,-1));
	if(pixel_coords.x<1920-1)	r=imageLoad(img_input, pixel_coords + ivec2(1,0));
	if(pixel_coords.y<1080-1)	u=imageLoad(img_input, pixel_coords + ivec2(0,1));

	va.xyz = normalize(col.rgb - vec3(0.5,0.5,0.5)) * col.a;
	va.a = col.a;
	vr = normalize(r.rgb - vec3(0.5,0.5,0.5)) * r.a;
	vl = normalize(l.rgb - vec3(0.5,0.5,0.5)) * l.a;
	vu = normalize(u.rgb - vec3(0.5,0.5,0.5)) * u.a;
	vd = normalize(d.rgb - vec3(0.5,0.5,0.5)) * d.a;

	//Advection
	float p = length(va.xy);
	if(pixel_coords.x>0 && pixel_coords.y>0 &&
		pixel_coords.x<1200-1 && pixel_coords.y<720-1) {
		vec2 pos = pixel_coords - p * va.xy;
		va = bilinearInterp(pos);
	}

	//Diffusion
	float alpha = 3.;
	float beta = 4. + alpha;
	va.xyz = (vl+vr+vd+vu+alpha*va.xyz)/beta;
	va.a = (l.a+r.a+d.a+u.a+alpha*va.a)/beta;

	//Pressure
	float hrdx = 0.25;
	va.xy -= hrdx*vec2(r.a-l.a, u.a-d.a);

	col.rgb = normalize(va.xyz)/2. + vec3(0.5,0.5,0.5);
	col.a = va.a;

	imageStore(img_output, pixel_coords, col);
	}