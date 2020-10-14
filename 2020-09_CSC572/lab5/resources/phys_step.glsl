#version 450
#extension GL_ARB_shader_storage_buffer_object : require

#define RASTERIZATION 8
#define BARYCENTER_RESOLUTION 1000

#define TOTAL_CELLS (RASTERIZATION * RASTERIZATION * RASTERIZATION)

struct star {
    vec3 position;
    uint cell;
    vec3 velocity;
    float mass;
    vec3 acceleration;
    uint next;
};

struct cell {
    ivec3 barycenter_int;
    uint mass;
    vec3 barycenter;
    uint head;
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
shared vec3 bounding_dims;

// Raster group data
shared cell cells[TOTAL_CELLS];

#define NIL stars.length()

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

// Star indices this worker scans through
const uint star_scan_start = linear_cell_index * stars.length() / TOTAL_CELLS;
const uint star_scan_end = (linear_cell_index + 1) * stars.length() / TOTAL_CELLS;

// Cell indices this worker scans through
const uint cell_scan_start = linear_cell_index * stars.length() / TOTAL_CELLS;
const uint cell_scan_end = (linear_cell_index + 1) * stars.length() / TOTAL_CELLS;

#define FOREACH_STAR for (uint i = star_scan_start; i < star_scan_end; i++)

vec3 gravity(vec3 p1, vec3 p2) {
    vec3 delta = p1 - p2;
    float r2 = dot(delta, delta);

    vec3 norm_delta = delta / sqrt(r2);
    float accel_mag = -1e-3 / r2;

    return accel_mag * norm_delta;
}

void disconnect_linked_lists() {
    FOREACH_STAR {
        stars[i].next = NIL;
    }
    cells[linear_cell_index].head = NIL;
}

void calculate_bounds() {
    // Phase 1: Calculate bounds for only this one's scan set
    vec3 min_b = 1 / vec3(0, 0, 0);
    vec3 max_b = -1 / vec3(0, 0, 0);

    FOREACH_STAR {
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

    // Calculate scale
    bounding_dims = max_bounds - min_bounds;
    barrier();
}

// Link stars to their cells
void rasterize(vec3 min_bounds, vec3 max_bounds) {
    cells[linear_cell_index].barycenter_int = ivec3(0, 0, 0);
    cells[linear_cell_index].mass = 0;
    barrier();

    FOREACH_STAR {
        vec3 star_position = stars[i].position;

        // Calculate the cell it's in
        vec3 cell_float = (star_position - min_bounds) / bounding_dims;
        uvec3 cell = uvec3(floor(cell_float * RASTERIZATION));
        uint cell_index = raster_pos_to_storage_index(cell);

        // Link star to cell
        stars[i].cell = cell_index;

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
    if (cells[linear_cell_index].mass > 0) {
        cells[linear_cell_index].barycenter = vec3(cells[linear_cell_index].barycenter_int) / cells[linear_cell_index].mass / BARYCENTER_RESOLUTION;
    }
}

void build_linked_lists() {
    FOREACH_STAR {
        stars[i].next = atomicExchange(cells[stars[i].cell].head, i);
    }
    barrier();
}

// Apply gravitational forces
void gravitate_stars_to_cells() {
    FOREACH_STAR {
        star a = stars[i];
        a.acceleration = vec3(0, 0, 0);

        // For each cell
        for (int j = 0; j < TOTAL_CELLS; j++) {
            cell b = cells[j];
            if (b.mass == 0 || b.mass == 1 || a.cell == j) {
                continue;
            }

            a.acceleration += gravity(a.position, b.barycenter) * b.mass;
        }
        stars[i].acceleration = a.acceleration;
    }
}

void gravitate_within_cells() {
    const uint head = cells[linear_cell_index].head;
    uint i = stars[head].next;
    uint i_count = 1;
    while (i != NIL && i_count < stars.length()) {
        uint j = head;
        uint j_count = 0;
        while (j != NIL && j_count < i_count && j_count < stars.length()) {
            vec3 gr = gravity(stars[i].position, stars[j].position);
            stars[i].acceleration -= gr * stars[j].mass;
            stars[j].acceleration += gr * stars[i].mass;
            j = stars[j].next;
            j_count++;
        }
        i = stars[i].next;
        i_count++;
    }
    barrier();
}

// Apply star acceleration
void integrate() {
    FOREACH_STAR {
        star self = stars[i];
        self.velocity += self.acceleration * dt;
        self.position += self.velocity * dt;
        stars[i] = self;
    }
}

void main() {
    disconnect_linked_lists();
    calculate_bounds();
    rasterize(min_bounds, max_bounds);
    build_linked_lists();
    gravitate_stars_to_cells();
    gravitate_within_cells();
    integrate();
}
