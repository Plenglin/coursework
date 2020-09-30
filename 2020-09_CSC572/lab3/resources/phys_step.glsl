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


// Reflects v over the unit vector.
vec3 reflect(vec3 v, vec3 over) {
    return v - 2 * dot(v, over) * over;
}

// Given two objects colliding with masses ma, mb; velocities va, vb; and collision unit normal N, returns the impulse applied to object a.
vec3 get_reflection_impulse(float ma, vec3 va, float mb, vec3 pb, vec3 N) {
    vec3 va_dir = reflect(va, N);

    if (mb < 0) {  // second object is static
        return ma * (va_dir - va);  // Pure reflection
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

float wall_dist = 5;

bool should_collide_circle_plane(sphere s, plane p) {
    float distance = dot(s.position - p.position, p.normal);
    return distance < s.radius &&  // Inside the plane
        dot(s.velocity, p.normal) < 0;  // Not leaving the plane
}

plane walls[] = {
{ vec3(0, -wall_dist, 0), vec3(0, 1, 0) },
{ vec3(0, 0, wall_dist), vec3(0, 0, -1) },
{ vec3(0, 0, -wall_dist), vec3(0, 0, 1) },
{ vec3(-wall_dist, 0, 0), vec3(1, 0, 0) },
{ vec3(wall_dist, 0, 0), vec3(-1, 0, 0) }
};

vec3 bounds_check(sphere self) {
    vec3 impulse = vec3(0, 0, 0);
    for (int i = 0; i < 5; i++) {
        if (should_collide_circle_plane(self, walls[i])) {
            impulse += get_reflection_impulse(self.mass, self.velocity, -1, vec3(0, 0, 0), walls[i].normal);
        }
    }
    return impulse;
}

void main() {
    index = gl_GlobalInvocationID.x;

    items[index].impulse = vec3(0, 0, 0);
    sphere self = items[index];

    // Calculate collisions. Note that we skip over 0.
    /*for (int i = 1; i < SPHERES_N; i++) {
        uint other_index = (index + i) % SPHERES_N;
        sphere other = items[other_index];
        vec3 impulse = collide(other);
        self.velocity += impulse;
        barrier();
    }*/

    self.impulse += bounds_check(self);
    self.impulse += self.mass * acceleration * dt;

    // Integration
    self.velocity += (self.impulse / self.mass);
    self.position += self.velocity * dt;

    // Store
    items[index].position = self.position;
    items[index].velocity = self.velocity;
}
