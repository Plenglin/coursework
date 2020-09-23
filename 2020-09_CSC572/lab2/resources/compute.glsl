#version 450 
#extension GL_ARB_shader_storage_buffer_object : require
layout(local_size_x = 1024, local_size_y = 1) in;	
layout (binding = 0, offset = 0) uniform atomic_uint ac;

//	for texture handling
layout(rgba8, binding = 0) uniform image2D img;		//input/output image
//		vec4 col=imageLoad(img_input, pixel_coords);
//		imageStore(img_output, pixel_coords, col);


//local group of shaders
layout (std430, binding=0) volatile buffer shader_data
{ 
	vec4 dataA[1024];
	ivec4 info[1];
};
uniform int sizeofbuffer;

void main() {
	// Get the index to operate on
	uint ai = gl_LocalInvocationID.x * 2;
	ai += info[0].z;  // used to select even/odd
	uint bi = ai + 1;

	// Comparison
	float a = dataA[ai].x;
	float b = dataA[bi].x;
	if (b < a) {
		// Incorrectly sorted
		dataA[ai].x = b;
		dataA[bi].x = a;
	} else {
		// Correctly sorted, do an increment
		atomicAdd(info[0].y, 1);
	}
}