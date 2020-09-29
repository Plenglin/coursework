#version 330 core

struct sphere {
    vec3 position;
    vec3 velocity;
    float mass;
    float radius;
    float _0;
    float _1;
};

layout(local_size_x = 100, local_size_y = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;
layout (std430, binding=0) volatile buffer shader_data {
    sphere items[100];
};

sphere self;

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

    // Position step
    self.position += self.velocity * 0.01;

    // Store
    items[index].position = self.position;
    items[index].velocity = self.velocity;
}
