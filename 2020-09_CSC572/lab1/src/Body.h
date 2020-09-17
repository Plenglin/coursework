//
// Created by astrid on 9/16/20.
//

#ifndef LAB3_BODY_h
#define LAB3_BODY_h


#include <glm/detail/type_mat.hpp>
#include <vector>
#include <functional>


class Body {
public:
    std::vector<Body*> satellites;
    float orbit_velocity;
    float orbit_phase;
    float orbit_radius;
    float rotation_velocity;
    float rotation_phase;
    Body(float orbitVelocity, float orbitRadius, float rotationVelocity, float orbitPhase = 0, float rotationPhase = 0);

    void add_satellite(Body *satellite);
    void update(float dt);
    void get_transform(glm::mat4 &mat);
    void foreach(const std::function<void(Body*)>& f);
};


#endif //LAB3_BODY_h