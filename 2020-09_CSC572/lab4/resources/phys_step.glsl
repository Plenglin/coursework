#version 450
#extension GL_ARB_shader_storage_buffer_object : require

#define RASTERIZATION 5
#define BARYCENTER_RESOLUTION 1000

#define TOTAL_CELLS (RASTERIZATION * RASTERIZATION * RASTERIZATION)

struct star {
    vec3 position;
    uint cell;
    vec3 velocity;
    float _;
};

struct cell {
    ivec3 barycenter_int;
    uint mass;
    vec3 acceleration;
    uint _;
    vec3 barycenter;
    uint _2;
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

// Raster group data
shared cell cells[TOTAL_CELLS];

uint raster_pos_to_storage_index(uvec3 pos) {
    return pos.x + RASTERIZATION * (pos.y + RASTERIZATION * pos.z);
}

uvec3 storage_index_to_raster_pos(uint i) {
    uvec3 pos;
    pos.x = i % RASTERIZATION;

    i /= RASTERIZATION;
    pos.y = i % RASTERIZATION;

    i /= RASTERIZATION;
    pos.z = i % RASTERIZATION;
    return pos;
}

uniform float dt;
uniform float G;

// Where this worker's cell index is
const uint linear_cell_index = raster_pos_to_storage_index(gl_GlobalInvocationID);

// Star indices to scan through during linear scan
const uint star_scan_start = linear_cell_index * stars.length() / TOTAL_CELLS;
const uint star_scan_end = (linear_cell_index + 1) * stars.length() / TOTAL_CELLS;

void calculate_bounds() {
    // Phase 1: Calculate bounds for only this one's linear scan set
    vec3 min_b = vec3(0, 0, 0);
    vec3 max_b = vec3(0, 0, 0);

    // For each star
    for (uint i = star_scan_start; i < star_scan_end; i++) {
        min_b = min(min_b, stars[i].position);
        max_b = max(max_b, stars[i].position);
    }
    intermediate_min_bounds[linear_cell_index] = min_b;
    intermediate_max_bounds[linear_cell_index] = max_b;
    barrier();

    // Phase 2: The leader aggregates the results. TODO: use binary tree aggregation
    if (linear_cell_index != 0) {
        barrier();
        return;
    }

    // Default min/max bounding value
    min_bounds = min_b;
    max_bounds = max_b;

    // For each cell, aggregate
    for (int i = 0; i < TOTAL_CELLS; i++) {
        min_bounds = min(min_bounds, intermediate_min_bounds[i]);
        max_bounds = max(max_bounds, intermediate_max_bounds[i]);
    }
    barrier();
}

// Assign stars to their cells
void rasterize(vec3 min_bounds, vec3 max_bounds) {
    vec3 world_to_raster_scale = RASTERIZATION / (max_bounds - min_bounds);

    cells[linear_cell_index].barycenter_int = ivec3(0, 0, 0);
    cells[linear_cell_index].mass = 0;
    barrier();

    // For each star
    for (uint i = star_scan_start; i < star_scan_end; i++) {
        vec3 star_position = stars[i].position;

        // Calculate the cell it's in
        vec3 cell_float = (star_position - min_bounds) * world_to_raster_scale;
        uvec3 cell = uvec3(floor(cell_float));
        uint cell_index = raster_pos_to_storage_index(cell);

        // Link star to cell
        stars[i].cell = cell_index;
        stars[i]._ = float(cell.x);

        // Increment cell mass
        atomicAdd(cells[cell_index].mass, 1);

        // Increment barycenter accumulator
        ivec3 int_pos = ivec3(BARYCENTER_RESOLUTION * star_position);
        atomicAdd(cells[cell_index].barycenter_int.x, int_pos.x);
        atomicAdd(cells[cell_index].barycenter_int.y, int_pos.y);
        atomicAdd(cells[cell_index].barycenter_int.z, int_pos.z);
    }
    barrier();

    // Convert sum to average
    cells[linear_cell_index].barycenter = vec3(cells[linear_cell_index].barycenter_int) / cells[linear_cell_index].mass / BARYCENTER_RESOLUTION;
}

// Apply gravitational forces
void gravitate_cells() {
    // Reset cell acceleration
    cells[linear_cell_index].acceleration = vec3(0, 0, 0);
    barrier();

    bool empty = cells[linear_cell_index].mass == 0;

    for (uint other_linear_cell_index = 0; other_linear_cell_index < TOTAL_CELLS; other_linear_cell_index++) {
        if (empty || other_linear_cell_index >= linear_cell_index) {
            barrier();
            barrier();
            continue;
        }

        cell a = cells[linear_cell_index];
        cell b = cells[other_linear_cell_index];

        vec3 delta = a.barycenter - b.barycenter;
        float r2 = dot(delta, delta);
        float Gr2 = 100 / r2;
        vec3 norm_delta = delta / sqrt(r2);

        vec3 accel_a = Gr2 * b.mass * norm_delta;
        vec3 accel_b = -Gr2 * a.mass * norm_delta;

        // Barriers to ensure no race conditions.
        cells[linear_cell_index].acceleration += accel_a;
        barrier();
        cells[other_linear_cell_index].acceleration += accel_b;
        barrier();
    }
}

// Apply star acceleration
void integrate() {
    for (uint i = star_scan_start; i < star_scan_end; i++) {
        star self = stars[i];
        vec3 acceleration = cells[self.cell].acceleration;
        self.velocity += acceleration * dt;
        self.position += self.velocity * dt;
        stars[i] = self;
    }
}

void main() {
    calculate_bounds();
    rasterize(vec3(0, 0, 0), vec3(5, 5, 5));
    //gravitate_cells();
    //integrate();
}
