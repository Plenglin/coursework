#version 450
#extension GL_ARB_shader_storage_buffer_object : require

#define RASTERIZATION 5
#define TOTAL_CELLS (RASTERIZATION * RASTERIZATION * RASTERIZATION)

struct star {
    vec3 position;
    uint cell;
    vec3 velocity;
    float _;
};

struct cell {
    vec3 barycenter;
    uint mass;
    vec3 acceleration;
    uint _;
};

layout(local_size_x = RASTERIZATION, local_size_y = RASTERIZATION, local_size_z = RASTERIZATION) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;
layout (std430, binding=0) volatile buffer shader_data {
    star stars[];
};

// Minimums and maximums found by each worker
shared vec3 intermediate_min_bounds[TOTAL_CELLS];
shared vec3 intermediate_max_bounds[TOTAL_CELLS];
// Global minimum and maximum
shared vec3 min_bounds;
shared vec3 max_bounds;
shared vec3 world_to_raster_scale;

// Raster group data
shared cell cells[TOTAL_CELLS];

uint linearize_raster_cell(uvec3 pos) {
    return pos.x + RASTERIZATION * (pos.y + RASTERIZATION * pos.z);
}

uniform float dt;
uniform float G;
const uint linear_index = linearize_raster_cell(gl_GlobalInvocationID);

// Result index to put this worker's results in during linear scan
const uint scan_block_index = linear_index / stars.length();

// Star indices to scan through during linear scan
const uint linear_scan_start = scan_block_index * RASTERIZATION;
const uint linear_scan_end = (scan_block_index + 1) * RASTERIZATION;

void calculate_bounds() {
    // Phase 1: Calculate bounds for only this one's linear scan set
    vec3 min_b = vec3(0, 0, 0);
    vec3 max_b = vec3(0, 0, 0);
    for (uint i = linear_scan_start; i < linear_scan_end; i++) {
        min_b = min(min_b, stars[i].position);
        max_b = max(max_b, stars[i].position);
    }
    intermediate_min_bounds[linear_index] = min_b;
    intermediate_max_bounds[linear_index] = max_b;
    barrier();

    // Phase 2: The leader aggregates the results. TODO: use binary tree aggregation
    if (linear_index != 0) {
        barrier();
        return;
    }
    min_bounds = min_b;
    max_bounds = max_b;
    for (int i = 0; i < TOTAL_CELLS; i++) {
        min_bounds = min(min_bounds, intermediate_min_bounds[i]);
        max_bounds = max(max_bounds, intermediate_max_bounds[i]);
    }
    world_to_raster_scale = (max_bounds - min_bounds) * RASTERIZATION;
    barrier();
}

// Scan stars for items inside this linear index
void rasterize() {
    for (uint i = linear_scan_start; i < linear_scan_end; i++) {
        uvec3 cell = uvec3((stars[i].position - min_bounds) / world_to_raster_scale);
        uint cell_index = linearize_raster_cell(cell);
        atomicAdd(cells[cell_index].mass, 1);
        stars[i].cell = cell_index;
    }
    barrier();
}

void main() {
    calculate_bounds();
    rasterize();

    // Calculate sphere collisions
/*
    for (uint other_index = 0; other_index < STARS_N; other_index++) {
        if (other_index >= index) {
            barrier();
            barrier();
            continue;
        }

        sphere b = items[other_index];
        sphere a = items[index];
        vec3 force = collide_sphere_sphere(a, b) * dt;

        if (dot(force, force) > 0) {
            vec3 impulse_a = (force + dampening(0.2, a.velocity)) * dt;
            vec3 impulse_b = (-force + dampening(0.2, b.velocity)) * dt;

            // Barriers to ensure no race conditions.
            items[index].impulse += impulse_a;
            barrier();
            items[other_index].impulse += impulse_b;
            barrier();
        } else {
            barrier();
            barrier();
        }
    }

    sphere self = items[index];
    self.impulse += bounds_check(self);
    self.impulse += self.mass * acceleration * dt;

    // Integration
    self.velocity += (self.impulse / self.mass);
    self.position += self.velocity * dt;

    // Store
    items[index].position = self.position;
    items[index].velocity = self.velocity;*/
}
