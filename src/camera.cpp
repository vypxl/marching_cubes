#include "camera.hpp"

Camera::Camera() {
    pos = glm::vec3(0.f, 0.f, 0.f);
    direction = glm::vec3(0.f, 0.f, -1.f);
    setModel(glm::mat4(1.f));
    setProjection(45, 16, 9);
}

