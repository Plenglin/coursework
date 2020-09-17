//
// Created by astrid on 9/16/20.
//

#ifndef LAB3_BODY_h
#define LAB3_BODY_h


#include <glm/detail/type_mat.hpp>
#include <vector>
#include <functional>
#include <glad/glad.h>
#include "Shape.h"


class Body {
public:
    std::vector<Body*> satellites;
    float orbit_velocity;
    float orbit_phase;
    float orbit_radius;
    float rotation_velocity;
    float rotation_phase;
    float scale;
    glm::mat4 transform;
    glm::mat4 personal_transform;
    std::shared_ptr<Shape> shape;
    GLuint texture;
    Body(float orbitVelocity, float orbitRadius, float rotationVelocity, float scale, float orbitPhase = 0, float rotationPhase = 0);

    void add_satellite(Body *satellite);
    void update(float dt);
    void foreach(const std::function<void(Body*)>& f);
    void draw(const std::shared_ptr<Program>& prog, glm::mat4 &P, glm::mat4 &V, glm::vec3 &campos);
};


#endif //LAB3_BODY_h