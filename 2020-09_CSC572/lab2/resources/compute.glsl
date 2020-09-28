#version 450 
#extension GL_ARB_shader_storage_buffer_object : require
#define WORKGROUP_SIZE 64

layout(local_size_x = WORKGROUP_SIZE, local_size_y = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;

//local group of shaders
layout (std430, binding=0) volatile buffer shader_data {
	ivec4 global_sorted;
	ivec4 even;
	vec4 items[];
};

shared bool group_sorted;
shared bool was_toggled;

void compare(uint ai, uint bi) {
	float a = items[ai].x;
	float b = items[bi].x;
	if (b < a) {
		// Incorrectly sorted
		items[bi].x = a;
		items[ai].x = b;
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
	uint count = items.length();

	uint group_start = gl_WorkGroupID.x * WORKGROUP_SIZE * 2;
	uint group_end = min((gl_WorkGroupID.x + 1) * WORKGROUP_SIZE * 2, count);

	// The index that this unit is centered around
	uint bi = index * 2;
	if (even.x == 0) {
		bi += WORKGROUP_SIZE;
	}
	if (bi >= count) return;

	uint ai = bi - 1;
	uint ci = bi + 1;
	bool run_even = index != 0 && group_start <= ai;
	bool run_odd = ci < group_end;

	was_toggled = false;
	barrier();

	for (int i = 0; i < count * count * 4; i++) {  // Loop limit so I don't freeze my computer
		// Set the global valid flag.
		if (gl_LocalInvocationID.x == 0) {
			group_sorted = true;
		}
		barrier();

		// Even
		if (run_even) {
			compare(ai, bi);
		}
		barrier();

		// Odd
		if (run_odd) {
			compare(bi, ci);
		}
		barrier();

		// End the loop if valid.
		if (group_sorted) {
			break;
		}
	}

	// This group did not start out sorted.
	if (gl_LocalInvocationID.x == 0 && was_toggled) {
		global_sorted.x = 0;
	}
}