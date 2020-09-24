#version 450 
#extension GL_ARB_shader_storage_buffer_object : require
#define WORKGROUP_SIZE 8

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;

//local group of shaders
layout (std430, binding=0) volatile buffer shader_data {
	ivec4 global_sorted;
	ivec4 even;

	ivec4 data_count;
	vec4 dataA[4096];
};

uniform int sizeofbuffer;
shared bool group_sorted;
shared bool was_toggled;

void compare(uint ai, uint bi) {
	float a = dataA[ai].x;
	float b = dataA[bi].x;
	if (b < a) {
		// Incorrectly sorted
		dataA[bi].x = a;
		dataA[ai].x = b;
		// Clear the valid flag. Note that no atomicity is needed here, since the only operation being done
		// before the checking barrier is writing, and all we care is that ANY of them are invalid.
		group_sorted = false;
		was_toggled = true;
	}
	// Correctly sorted, do nothing
}

void main() {
	// Get the global index of this unit
	uint index = gl_GlobalInvocationID.x;
	uint count = data_count.x;

	// The index that this unit is centered around
	uint bi = index * 2;
	if (bi >= count) return;

	uint ai = bi - 1;
	uint ci = bi + 1;
	bool non_first = ai < bi;
	bool non_last = ci < count;

	was_toggled = false;
	barrier();

	for (int i = 0; i < count * count * 4; i++) {  // Loop limit so I don't freeze my computer
		// Set the global valid flag.
		if (gl_LocalInvocationID.x == 0) {
			group_sorted = true;
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

		// End the loop if valid.
		if (group_sorted) {
			break;
		}
	}

	// This group did not start out sorted.
	if (gl_LocalInvocationID.x == 0 && was_toggled) {
		global_sorted.x = 1;
	}
}