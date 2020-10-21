#version 450
#extension GL_ARB_shader_storage_buffer_object : require

#define RASTERIZATION 15
#define L1_WIDTH 5
#define L2_WIDTH 3
#define TOTAL_CELLS (RASTERIZATION * RASTERIZATION * RASTERIZATION)
#define L1_CELLS RASTERIZATION
#define L2_CELLS (L2_WIDTH * L2_WIDTH * L2_WIDTH * 27)

#define BARYCENTER_RESOLUTION 1000
#define WORKERS 512

struct star {
    vec3 position;
    uint cell;

    vec3 velocity;
    float mass;

    vec3 acceleration;
    uint next;

    vec3 test;
    float _;
};

struct cell {
    vec3 acceleration;
    float mass;

    vec3 barycenter;
    uint head;

    uint array_list_start;
    uint count;
    uint _1;
    uint _2;
};

layout(local_size_x = WORKERS, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;
layout (std430, binding=0) volatile buffer shader_data {
    // Raster group data
    cell cells[TOTAL_CELLS];
    cell l1[L1_CELLS];
    cell l2[L2_CELLS];
    star stars[];
};

// Minimums and maximums found by each worker
shared vec3 intermediate_vec[WORKERS];

// Global minimum and maximum
shared vec3 mean_pos;
shared vec3 stdev_pos;
shared vec3 dev_limits;

const uint STARS_COUNT = stars.length();
#define NIL STARS_COUNT
#define BOUNDS_STDEVS 3

uint raster_pos_to_storage_index(uvec3 pos, uint width) {
    return pos.x + width * (pos.y + width * pos.z);
}

uvec3 storage_index_to_raster_pos(uint i, uint width) {
    uvec3 pos;
    pos.x = i % width;

    i /= width;
    pos.y = i % width;

    i /= width;
    pos.z = i;
    return pos;
}

uniform float dt;
uniform float G;
uniform float centeredness;

// Where this worker's cell index is
const uint worker_index = gl_GlobalInvocationID.x;

// Star indices this worker scans through
const uint star_scan_start = worker_index * STARS_COUNT / WORKERS;
const uint star_scan_end = (worker_index + 1) * STARS_COUNT / WORKERS;

// Cell indices this worker scans through
const uint cell_scan_start = worker_index * TOTAL_CELLS / WORKERS;
const uint cell_scan_end = (worker_index + 1) * TOTAL_CELLS / WORKERS;

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
    for (uint i = cell_scan_start; i < cell_scan_end; i++) {
        cells[i].head = NIL;
        cells[i].count = 0;
    }
    barrier();
}

void calculate_bounds2() {
    if (worker_index == 0) {
        vec3 sum = vec3(0, 0, 0);
        float mass_sum = 0;
        for (uint i = 0; i < STARS_COUNT; i++) {
            mass_sum += stars[i].mass;
            sum += stars[i].position * stars[i].mass;
        }
        mean_pos = sum / mass_sum;

        float dev2 = 0;
        for (uint i = 0; i < STARS_COUNT; i++) {
            vec3 d = stars[i].position - mean_pos;
            dev2 += dot(d, d);
        }
        float stdev = sqrt(dev2 / STARS_COUNT);
        stdev_pos = vec3(stdev, stdev, stdev);

        dev_limits = BOUNDS_STDEVS * stdev_pos;
    }
    barrier();
}

// Link stars to their cells
void rasterize() {
    cells[worker_index].mass = 0;
    barrier();

    // For each cell
    for (uint i = star_scan_start; i < star_scan_end; i++) {
        vec3 star_position = stars[i].position;

        // Calculate the cell it's in
        vec3 norm_pos = (star_position - mean_pos) / dev_limits;
        vec3 skew_pos = sign(norm_pos) * pow(abs(norm_pos), vec3(centeredness, centeredness, centeredness));
        vec3 cube_pos = (skew_pos + 1) / 2;
        uvec3 cell = uvec3(floor(cube_pos * RASTERIZATION));
        stars[i].test = floor(cube_pos * RASTERIZATION);
        uint cell_index = raster_pos_to_storage_index(cell, RASTERIZATION);

        // Ensure that there exists a cell containing this star
        if (!(0 <= cell_index && cell_index < TOTAL_CELLS)) {
            stars[i].cell = NIL;
            continue;
        }

        // Link star to cell
        stars[i].cell = cell_index;
        stars[i].next = atomicExchange(cells[cell_index].head, i);
        atomicAdd(cells[cell_index].count, 1);
    }

    barrier();

    // Compute cell data
    for (uint c = cell_scan_start; c < cell_scan_end; c++) {
        const uint head = cells[c].head;
        vec3 wpos_sum = vec3(0, 0, 0);
        float mass_sum = 0;
        for (uint i = head; i != NIL; i = stars[i].next) {
            wpos_sum += stars[i].mass * stars[i].position;
            mass_sum += stars[i].mass;
        }
        cells[c].mass = mass_sum;
        if (mass_sum > 0) {
            cells[c].barycenter = wpos_sum / mass_sum;
        }
    }
    barrier();
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
        for (uint i = head; i != NIL; i = stars[i].next) {
            if (self_index != i) {
                vec3 gr = gravity(stars[self_index].position, stars[i].position);
                stars[self_index].acceleration += gr * stars[i].mass;
            }
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
    calculate_bounds2();
    rasterize();
    gravitate_stars_to_cells();
    gravitate_within_cells();
    integrate();
}
