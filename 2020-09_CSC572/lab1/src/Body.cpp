//
// Created by astrid on 9/16/20.
//

#include <glm/gtc/matrix_transform.hpp>
#include "Body.h"

void Body::add_satellite(Body *satellite) {
    satellites.push_back(satellite);
}

void Body::update(float dt) {
    rotation_phase += rotation_velocity * dt;
    orbit_phase += orbit_velocity * dt;
    for (auto s : satellites) {
        s->update(dt);
    }
}

Body::Body(float orbitVelocity, float orbitRadius, float rotationVelocity, float orbitPhase, float rotationPhase)
        : orbit_velocity(orbitVelocity), orbit_phase(orbitPhase), orbit_radius(orbitRadius),
          rotation_velocity(rotationVelocity), rotation_phase(rotationPhase) {}

static const auto Z = glm::vec3(0, 0, 1);

void Body::get_transform(glm::mat4 &mat) {
    glm::rotate(mat, rotation_phase, Z);
    glm::translate(mat, glm::vec3(orbit_radius, 0, 0));
    glm::rotate(mat, orbit_radius, Z);
}

