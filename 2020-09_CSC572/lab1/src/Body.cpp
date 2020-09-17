//
// Created by astrid on 9/16/20.
//

#include <glm/gtc/matrix_transform.hpp>
#include "Body.h"
#include "Program.h"

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
          rotation_velocity(rotationVelocity), rotation_phase(rotationPhase), scale(scale) {}

void Body::foreach(const std::function<void(Body*)>& f) {
    f(this);
    for (auto s : satellites) {
        s->foreach(f);
    }
}

void Body::draw(const std::shared_ptr<Program>& prog, glm::mat4 &P, glm::mat4 &V, glm::vec3 &campos) {
    prog->bind();

    //send the matrices to the shaders
    glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
    glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &transform[0][0]);
    glUniform3fv(prog->getUniform("campos"), 1, &campos[0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    shape->draw(prog, false);

    prog->unbind();
}
