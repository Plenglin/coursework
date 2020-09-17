//
// Created by astrid on 9/16/20.
//

#include <glm/gtc/matrix_transform.hpp>
#include "Body.h"

#define BASIS_UP glm::vec3(0, 1, 0)

void Body::add_satellite(Body *satellite) {
    satellites.push_back(satellite);
}

void Body::update(float dt) {
    rotation_phase += rotation_velocity * dt;
    orbit_phase += orbit_velocity * dt;

    auto position_transform = glm::rotate(glm::mat4(1.0f), orbit_phase, BASIS_UP) *
                              glm::translate(glm::mat4(1.0f), glm::vec3(orbit_radius, 0, 0));
    transform = position_transform * glm::rotate(glm::mat4(1.0f), rotation_phase, BASIS_UP);

    for (auto s : satellites) {
        s->update(dt);
        s->transform = position_transform * s->transform;
    }
}

Body::Body(float orbitVelocity, float orbitRadius, float rotationVelocity, float orbitPhase, float rotationPhase)
        : orbit_velocity(orbitVelocity), orbit_phase(orbitPhase), orbit_radius(orbitRadius),
          rotation_velocity(rotationVelocity), rotation_phase(rotationPhase) {}

void Body::foreach(const std::function<void(Body*)>& f) {
    f(this);
    for (auto s : satellites) {
        s->foreach(f);
    }
}
