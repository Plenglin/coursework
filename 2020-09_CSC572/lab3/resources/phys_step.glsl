#version 330 core

struct sphere {
    vec3 position;
    vec3 velocity;
    float mass;
    float radius;
};

layout(local_size_x = 100, local_size_y = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;
layout (std430, binding=0) volatile buffer items_block {
    sphere items[100];
};

sphere self;

uniform float dt;
uniform vec3 acceleration;

// Projects v onto the unit vector onto.
float project_unit(vec3 v, vec3 onto) {
    return dot(onto, v);
}

// Reflects v over the unit vector over.
vec3 reflect(vec3 v, vec3 over) {
    vec3 proj = norm(v) * project_unit(v, over);
    vec3 step = proj - v;
    return v + 2 * step;
}

// Returns the impulse on self caused by other.
vec3 collide(sphere other) {
    vec3 dpos = other.position - self.position;
    float d2 = dot(dpos, dpos);
    float radius_sum = self.radius + other.radius;
    if (d2 > radius_sum * radius_sum) {
        return vec3(0, 0, 0);
    }

    vec3 normal = norm(dpos);

    float self_v1 = length(self.velocity);
    float other_v1 = length(other.velocity);

    vec3 self_v2_dir = reflect(self.velocity, normal);
    //vec3 other_v2_dir = reflect(other.velocity, normal);

    vec3 new_velocity = norm(self_v2_dir) * other_v1;
    return new_velocity - self.velocity;
}

void bounds_check() {
    if ((self.position.y-self.radius) < -5.0)
    self.velocity.y *= -1.0;

    if ((self.position.x - self.radius) < -5.0)
    self.velocity.x *= -1.0;
    if ((self.position.x + self.radius) > 5.0)
    self.velocity.x *= -1.0;

    if ((self.position.z - self.radius) < -5.0)
    self.velocity.z *= -1.0;
    if ((self.position.z + self.radius) < +5.0)
    self.velocity.z *= -1.0;
}

void main() {
    uint index = gl_GlobalInvocationID.x;

    // Load from memory
    self = items[index];

    // Calculate collisions. Note that we skip over 0.
    for (int i = 1; i < 100; i++) {
        uint other_index = (index + i) % 100;
        sphere other = items[other_index];
        vec3 impulse = collide(other);
        self.velocity += impulse;
        barrier();
    }

    // Integration
    self.velocity += acceleration * dt;
    self.position += self.velocity * dt;

    bounds_check();

    // Store
    items[index].position = self.position;
    items[index].velocity = self.velocity;
}