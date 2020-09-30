#version 450
#extension GL_ARB_shader_storage_buffer_object : require

#define SPHERES_N 100

// Assume all objects are rubbery and act like ideal springs.

struct sphere {
    vec3 position;
    float mass;
    vec3 velocity;
    float radius;
    vec3 impulse;
    float _0;  // padding
};

struct plane {
    vec3 position;
    vec3 normal;
};

layout(local_size_x = SPHERES_N, local_size_y = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;
layout (std430, binding=0) volatile buffer shader_data {
    sphere items[SPHERES_N];
};

uint index;
sphere self;

uniform float dt;
uniform vec3 acceleration;


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
    return 1000 * penetration * normal;
}

float wall_dist = 5;

vec3 collide_sphere_plane(sphere s, plane p) {
    float distance = dot(s.position - p.position, p.normal);
    if (distance < s.radius) {   // Inside the plane
        // it's a spring!
        float k = 1000;
        return -k * (distance - s.radius) * p.normal;
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
    index = gl_GlobalInvocationID.x;

    sphere self = items[index];
    self.impulse = vec3(0, 0, 0);

    // Calculate collisions. Note that we skip over 0.
    for (int i = 1; i < SPHERES_N; i++) {
        uint other_index = (index + i) % SPHERES_N;
        sphere other = items[other_index];
        vec3 impulse = collide_sphere_sphere(self, other) * dt;

        self.impulse += impulse;

        barrier();
    }

    self.impulse += bounds_check(self);
    self.impulse += self.mass * acceleration * dt;

    // Integration
    self.velocity += (self.impulse / self.mass);
    self.position += self.velocity * dt;

    // Store
    items[index].position = self.position;
    items[index].velocity = self.velocity;
}
