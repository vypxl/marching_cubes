#ifndef __camera_hpp_
#define __camera_hpp_

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Camera {
    const glm::vec3 up = glm::vec3(0.f, 1.f, 0.f);
private:
    glm::vec3 pos;
    glm::vec3 direction;
    float fov;

    glm::mat4 projection;
    glm::mat4 model;

public:
    Camera();

    inline glm::mat4 getMVP() { return projection * glm::lookAt(pos, pos + direction, up) * model; }
    inline void print() { std::cout << direction.x << " | " << direction.y << " | " << direction.z << std::endl; }

    template <class T1, class T2, class T3>
    inline void setProjection(T1 fov, T2 width, T3 height) { projection = glm::perspective(glm::radians((float) fov), (float) width / (float) height, 0.1f, 100.f); }
    inline void setModel(const glm::mat4 &new_model) { model = new_model; }

    inline void setDirection(const glm::vec3 &dir) { direction = glm::normalize(dir); }
    inline void lookAt(const glm::vec3 &target) { direction = glm::normalize(target - pos); }
    inline void setRotation(float rx, float ry) { direction = glm::vec3(cos(rx) * cos(ry), sin(ry), sin(rx) * cos(ry)); }

    inline void moveTo(const glm::vec3 &new_pos) { pos = new_pos; }
    inline void move(const glm::vec3 &offset) { pos += offset; }
};

#endif
