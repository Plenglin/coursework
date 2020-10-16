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

    uint _;
    uint _1;
    uint _2;
    uint _3;
};

struct cell {
    ivec3 barycenter_int;
    uint mass;

    vec3 barycenter;
    uint head;

    uint array_list_start;
    uint count;
    uint _1;
    uint _2;
};

layout(local_size_x = RASTERIZATION, local_size_y = RASTERIZATION, local_size_z = RASTERIZATION) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;
layout (std430, binding=0) volatile buffer shader_data {
    star stars[];
};

// Minimums and maximums found by each worker
shared vec3 intermediate_vec[TOTAL_CELLS];

// Global minimum and maximum
shared vec3 mean_pos;
shared vec3 stdev_pos;
shared vec3 max_bounds;
shared vec3 min_bounds;
shared vec3 bounding_dims;
shared uint max_cell_density;

// Raster group data
shared cell cells[TOTAL_CELLS];

const uint STARS_COUNT = stars.length();
#define NIL STARS_COUNT
#define BOUNDS_STDEVS 2

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
const uint star_scan_start = linear_cell_index * STARS_COUNT / TOTAL_CELLS;
const uint star_scan_end = (linear_cell_index + 1) * STARS_COUNT / TOTAL_CELLS;

// Cell indices this worker scans through
const uint cell_scan_start = linear_cell_index * STARS_COUNT / TOTAL_CELLS;
const uint cell_scan_end = (linear_cell_index + 1) * STARS_COUNT / TOTAL_CELLS;

vec3 gravity(vec3 p1, vec3 p2) {
    vec3 delta = p1 - p2;
    float r2 = dot(delta, delta);

    vec3 norm_delta = delta / sqrt(r2);
    float accel_mag = -G / r2;

    return accel_mag * norm_delta;
}

void disconnect_linked_lists() {
    for (uint i = star_scan_start; i < star_scan_end; i++) {
        stars[i].next = NIL;
    }
    cells[linear_cell_index].head = NIL;
    cells[linear_cell_index].count = 0;
}

void calculate_bounds() {
    {
        // Workers calculate sum of workset positions
        vec3 sum = vec3(0, 0, 0);
        for (uint i = star_scan_start; i < star_scan_end; i++) {
            sum += stars[i].position;
        }
        intermediate_vec[linear_cell_index] = sum;
        barrier();
    }

    // Leader aggregates sums to get the mean
    if (linear_cell_index == 0) {
        vec3 sum = vec3(0, 0, 0);
        for (uint i = star_scan_start; i < star_scan_end; i++) {
            sum += intermediate_vec[linear_cell_index];
        }
        mean_pos = sum / STARS_COUNT;
    }
    barrier();

    {
        // Workers calculate sum of squared deviations of workset positions
        float sum = 0;
        for (uint i = star_scan_start; i < star_scan_end; i++) {
            vec3 dev = stars[i].position - mean_pos;
            sum += dot(dev, dev);
        }
        intermediate_vec[linear_cell_index].x = sum;
        barrier();
    }
    barrier();

    // Leader aggregates sum squared diffs to get the mean
    if (linear_cell_index == 0) {
        float sum = 0;
        for (uint i = star_scan_start; i < star_scan_end; i++) {
            sum += intermediate_vec[linear_cell_index].x;
        }
        float stdev = sqrt(sum / STARS_COUNT);
        stdev_pos = vec3(stdev, stdev, stdev);

        max_bounds = mean_pos + BOUNDS_STDEVS * stdev_pos;
        min_bounds = mean_pos - BOUNDS_STDEVS * stdev_pos;

        // Calculate scale
        bounding_dims = max_bounds - min_bounds;
    }

    barrier();
}

// Link stars to their cells
void rasterize(vec3 min_bounds, vec3 max_bounds) {
    cells[linear_cell_index].barycenter_int = ivec3(0, 0, 0);
    cells[linear_cell_index].mass = 0;
    barrier();

    for (uint i = star_scan_start; i < star_scan_end; i++) {
        vec3 star_position = stars[i].position;

        // Calculate the cell it's in
        vec3 cell_float = (star_position - min_bounds) / bounding_dims;
        uvec3 cell = uvec3(floor(cell_float * RASTERIZATION));
        uint cell_index = raster_pos_to_storage_index(cell);

        // Ensure that there exists a cell containing this star
        if (!(0 <= cell_index && cell_index < TOTAL_CELLS)) {
            stars[i].cell = NIL;
            continue;
        }

        // Link star to cell
        stars[i].cell = cell_index;
        stars[i].next = atomicExchange(cells[cell_index].head, i);
        atomicAdd(cells[cell_index].count, 1);

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

// Apply gravitational forces
void gravitate_stars_to_cells() {
    for (uint i = star_scan_start; i < star_scan_end; i++) {
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
    for (uint self_index = star_scan_start; self_index < star_scan_end; self_index++) {
        const uint head = cells[stars[self_index].cell].head;
        uint i = head;
        while (i != NIL) {
            if (self_index != i) {
                vec3 gr = gravity(stars[self_index].position, stars[i].position);
                stars[self_index].acceleration += gr * stars[i].mass;
            }

            i = stars[i].next;
        }
    }
}

// Apply star acceleration
void integrate() {
    for (uint i = star_scan_start; i < star_scan_end; i++) {
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
    gravitate_stars_to_cells();
    gravitate_within_cells();
    integrate();
}
