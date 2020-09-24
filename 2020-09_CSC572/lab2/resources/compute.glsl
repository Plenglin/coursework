#version 450 
#extension GL_ARB_shader_storage_buffer_object : require
layout(local_size_x = 512, local_size_y = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;

//local group of shaders
layout (std430, binding=0) volatile buffer shader_data
{
	ivec4 info[2];
	vec4 dataA[4096];
};
uniform int sizeofbuffer;

void compare(uint ai, uint bi) {
	float a = dataA[ai].x;
	float b = dataA[bi].x;
	if (b < a) {
		// Incorrectly sorted
		dataA[bi].x = a;
		dataA[ai].x = b;
		// Clear the valid flag. Note that no atomicity is needed here, since the only operation being done
		// before the checking barrier is writing, and all we care is that ANY of them are invalid.
		info[1].x = 0;
	}
	// Correctly sorted, do nothing
}

void main() {
	// Get the index of this unit
	uint index = gl_LocalInvocationID.x;

	uint count = info[0].x;
	uint bi = index * 2;
	if (bi >= count) return;

	uint ai = bi - 1;
	uint ci = bi + 1;
	bool non_first = ai < bi;
	bool non_last = ci < count;

	for (int i = 0; i < count * count * 4; i++) {  // Loop limit so I don't freeze my computer
		// Set the global valid flag.
		if (index == 0) {
			info[1].x = 1;
		}
		barrier();

		// Odd
		if (non_last) {
			compare(bi, ci);
		}
		barrier();

		// Even
		if (non_first) {
			compare(ai, bi);
		}
		barrier();

		// Break if valid.
		if (info[1].x != 0) {
			break;
		}
	}

}