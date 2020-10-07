#version 450
#extension GL_ARB_shader_storage_buffer_object : require

#define STARS_N 100

// Assume all objects are rubbery and act like ideal springs.

struct sphere {
    vec3 position;
    float mass;
    vec3 velocity;
    uint cell;  // Singly-linked list. -1 is nil.
    vec3 acceleration;
    uint _;
};

struct cell {
    vec3 barycenter;
    uint mass;
    vec3 acceleration;
    uint head;  // Singly-linked list. -1 is nil.
};

layout(local_size_x = STARS_N, local_size_y = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;
layout (std430, binding=0) volatile buffer shader_data {
    sphere items[];
};

uniform float dt;
uniform float G;

void main() {
    uint index = gl_GlobalInvocationID.x;

    // Reset impulse
    items[index].acceleration = vec3(0, 0, 0);
    barrier();

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
