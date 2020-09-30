#version 450
#extension GL_ARB_shader_storage_buffer_object : require

#define SPHERES_N 100

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

// Given two objects colliding with masses ma, mb; velocities va, vb; and collision unit normal N, returns the impulse applied to object a.
vec3 get_reflection_force(float ma, vec3 va, float mb, vec3 pb, vec3 N) {
    vec3 va_dir = reflect(va, N);

    if (isinf(mb)) {  // second object is static
        return va_dir;  // Pure reflection
    }

    // For now, assume ma = mb.

    //float lva = length(va);
    float lpb = length(pb);

    //vec3 other_v2_dir = reflect(other.velocity, normal);

    vec3 expected_va2 = normalize(va_dir) * lpb;
    return ma * (expected_va2 - va);
}

// Let's just blatantly copy box2d's modeling of this whole situation! https://www.iforce2d.net/b2dtut/collision-anatomy

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

vec3 collide_circle_plane(sphere s, plane p) {
    float distance = dot(s.position - p.position, p.normal);
    if (distance < s.radius) {
        //return get_reflection_force(s.mass, s.velocity, 1 / 0, vec3(0, 0, 0), p.normal);
    }
    return vec3(0, 0, 0);
}

plane bottom = {
    vec3(0, 0, 0),
    vec3(0, 1, 0)
};

void bounds_check(sphere self) {
    self.impulse += collide_circle_plane(self, bottom);

    /*if ((self.position.x - self.radius) < -wall_dist){
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
    }*/
}

void main() {
    index = gl_GlobalInvocationID.x;

    items[index].impulse = vec3(0, 0, 0);
    //sphere self = items[index];

    // Calculate collisions. Note that we skip over 0.
    /*for (int i = 1; i < SPHERES_N; i++) {
        uint other_index = (index + i) % SPHERES_N;
        sphere other = items[other_index];
        vec3 impulse = collide(other);
        self.velocity += impulse;
        barrier();
    }*/

    //bounds_check(self);

    // Integration
    //self.velocity += (self.impulse / self.mass) * dt;
    //self.position += self.velocity * dt;

    // Store
    //items[index].position = self.position;
    //items[index].velocity = self.velocity;
}
