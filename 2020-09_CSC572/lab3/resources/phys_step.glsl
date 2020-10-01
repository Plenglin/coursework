#version 450
#extension GL_ARB_shader_storage_buffer_object : require

#define SPHERES_N 100
#define MAX_COLLISIONS SPHERES_N * (SPHERES_N - 1) / 2
#define CALCULATIONS MAX_COLLISIONS / SPHERES_N

// Assume all objects are rubbery and act like ideal springs.
// Also, "lower triangular collision space" is every element in [0, SPHERES_N) x [0, SPHERES_N) such that (a, b) -> b < a

struct sphere {
    vec3 position;
    float mass;
    vec3 velocity;
    float radius;
};

struct plane {
    vec3 position;
    vec3 normal;
};

layout(local_size_x = SPHERES_N, local_size_y = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;
layout (std430, binding=0) volatile buffer shader_data {
    sphere items[SPHERES_N];
    vec3 collision_component_forces[MAX_COLLISIONS];
};

uniform float dt;
uniform vec3 acceleration;

// Maps t in [0, MAX_COLLISIONS) to lower triangular collision space.
uvec2 task_to_pair(uint t) {
    uint b = uint(0.5 * (sqrt(1 - 8 * t) - 1));
    uint a = t - b;
    return uvec2(a, b);
}

// Maps from lower triangular collision space to [0, MAX_COLLISIONS).
uint pair_to_task(uint a, uint b) {
    return b * (b + 1) / 2 + a;
}

// Compressible fluid friction. Probably not the most representative for bouncy balls, but it gets the job done.
vec3 dampening(float k, vec3 velocity) {
    return k * -velocity * velocity;
}

// Returns the impulse on a.
vec3 collide_sphere_sphere(sphere a, sphere b) {
    vec3 dpos = a.position - b.position;
    float d2 = dot(dpos, dpos);
    float radius_sum = a.radius + b.radius;

    if (d2 > radius_sum * radius_sum) {  // Not inside each other
        return vec3(0, 0, 0);
    }

    // Inside each other
    float d = sqrt(d2);
    float penetration = radius_sum - d;
    vec3 normal = dpos / d;
    return 10000 * penetration * normal + dampening(0.01, a.velocity - b.velocity);
}

float wall_dist = 5;

vec3 collide_sphere_plane(sphere s, plane p) {
    float distance = dot(s.position - p.position, p.normal);
    if (distance < s.radius) {   // Inside the plane
        // it's a spring!
        float k = 10000;
        return -k * (distance - s.radius) * p.normal + dampening(0.05, s.velocity);
    }
    return vec3(0, 0, 0);
}

plane walls[] = {
{ vec3(0, wall_dist, 0), vec3(0, -1, 0) },
{ vec3(0, -wall_dist, 0), vec3(0, 1, 0) },
{ vec3(0, 0, wall_dist), vec3(0, 0, -1) },
{ vec3(0, 0, -wall_dist), vec3(0, 0, 1) },
{ vec3(-wall_dist, 0, 0), vec3(1, 0, 0) },
{ vec3(wall_dist, 0, 0), vec3(-1, 0, 0) }
};

vec3 bounds_check(sphere self) {
    vec3 force = vec3(0, 0, 0);
    for (int i = 0; i < 6; i++) {
        force += collide_sphere_plane(self, walls[i]);
    }
    return force * dt;
}

void main() {
    uint index = gl_GlobalInvocationID.x;

    uint tasks_start = index * CALCULATIONS;
    uint tasks_end = tasks_start + CALCULATIONS;

    // Calculate and store sphere collisions
    for (uint i = tasks_start; i < tasks_end; i++) {
        uvec2 pair = task_to_pair(i);  // evenly distributes tasks in lower triangular collision space

        sphere a = items[pair.x];
        sphere b = items[pair.y];
        collision_component_forces[i] = collide_sphere_sphere(a, b);
    }
    barrier();

    // Sum this sphere's collision force vectors
    vec3 collison_force_sum = vec3(0, 0, 0);
    for (uint b = 0; b < SPHERES_N; b++) {
        if (b == index) continue;
        if (b < index) {  // lower triangle
            uint task = pair_to_task(index, b);
            collison_force_sum += collision_component_forces[task];
        } else {  // upper triangle
            uint task = pair_to_task(b, index);
            collison_force_sum -= collision_component_forces[task];
        }
    }
    vec3 impulse = collison_force_sum * dt;

    sphere self = items[index];
    impulse += bounds_check(self);
    impulse += self.mass * acceleration * dt;

    // Integration
    self.velocity += (impulse / self.mass);
    self.position += self.velocity * dt;

    // Store
    items[index].position = self.position;
    items[index].velocity = self.velocity;
}
