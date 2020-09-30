#version 450
#extension GL_ARB_shader_storage_buffer_object : require

struct sphere {
    vec3 position;
    vec3 velocity;
    float mass;
    float radius;
};

layout(local_size_x = 100, local_size_y = 1) in;
layout (binding = 0, offset = 0) uniform atomic_uint ac;
layout (std430, binding=0) volatile buffer shader_data {
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
    vec3 proj = normalize(v) * project_unit(v, over);
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

    vec3 normal = normalize(dpos);

    float self_v1 = length(self.velocity);
    float other_v1 = length(other.velocity);

    vec3 self_v2_dir = reflect(self.velocity, normal);
    //vec3 other_v2_dir = reflect(other.velocity, normal);

    vec3 new_velocity = normalize(self_v2_dir) * other_v1;
    return new_velocity - self.velocity;
}

float wall_dist = 3;

void bounds_check() {
    if ((self.position.y - self.radius) < -wall_dist) {
        self.velocity.y = abs(self.velocity.y);
    }

    if ((self.position.x - self.radius) < -wall_dist){
        self.velocity.x = abs(self.velocity.x);
    }
    if ((self.position.x + self.radius) > wall_dist){
        self.velocity.x = -abs(self.velocity.x);
    }

    if ((self.position.z - self.radius) < -wall_dist){
        self.velocity.z = abs(self.velocity.z);
    }
    if ((self.position.z + self.radius) < +wall_dist) {
        self.velocity.z = -abs(self.velocity.z);
    }
}

void main() {
    uint index = gl_GlobalInvocationID.x;

    // Load from memory
    self = items[index];

    /*
    // Calculate collisions. Note that we skip over 0.
    for (int i = 1; i < 100; i++) {
        uint other_index = (index + i) % 100;
        sphere other = items[other_index];
        vec3 impulse = collide(other);
        self.velocity += impulse;
        barrier();
    }*/

    // Integration
    self.velocity += acceleration * dt;
    self.position += self.velocity * dt;

    bounds_check();

    // Store
    items[index].position = self.position;
    items[index].velocity = self.velocity;
}
