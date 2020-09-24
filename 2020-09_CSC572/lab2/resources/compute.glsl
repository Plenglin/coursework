#version 450 
#extension GL_ARB_shader_storage_buffer_object : require
layout(local_size_x = 1024, local_size_y = 1) in;	
layout (binding = 0, offset = 0) uniform atomic_uint ac;

//local group of shaders
layout (std430, binding=0) volatile buffer shader_data
{ 
	vec4 dataA[1024];
	ivec4 info[1];
};
uniform int sizeofbuffer;

uint count = 0;

void compare(uint ai, uint bi) {
	float a = dataA[ai].x;
	float b = dataA[bi].x;
	if (b < a) {
		// Incorrectly sorted
		dataA[bi].x = a;
		dataA[ai].x = b;
	} else {
		// Correctly sorted, do an increment
		count++;
	}
}

void main() {
	// Get the index of this unit
	uint index = gl_GlobalInvocationID.x;

	uint total = info[0].x;
	uint bi = index * 2;
	uint ai = bi - 1;
	uint ci = bi + 1;

	for (int i = 0; i < 10; i++) {
		count = 0;
		// Odd
		if (bi < total) {
			compare(bi, ci);
		}
		barrier();

		// Even
		if (index != 0) {
			compare(ai, bi);
		}
		barrier();
	}

}