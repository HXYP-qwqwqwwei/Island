//
// Created by HXYP on 2023/3/16.
//

#include "util/Camera.h"
using namespace glm;

const vec3 Camera::VERTICAL_UP = CoordAxis::YP;

mat4 Camera::getView() {
//    this->update();
//    return this->view;
    glm::vec3 eye   = this->pos;

//    this->right = glm::normalize(glm::cross(VERTICAL_UP, this->focal));
//    this->up = glm::cross(this->focal, right);
    return {
            glm::vec4(this->right.x, this->up.x, this->focal.x, 0),
            glm::vec4(this->right.y, this->up.y, this->focal.y, 0),
            glm::vec4(this->right.z, this->up.z, this->focal.z, 0),
            glm::vec4(-glm::dot(right, eye), -glm::dot(up, eye), -glm::dot(this->focal, eye), 1)
    };
}

void Camera::update() {
    this->right = glm::normalize(glm::cross(VERTICAL_UP, this->focal));
    this->up = glm::cross(this->focal, right);
}

Camera::Camera(const vec3 &pos, const vec3& focal): pos(pos), focal(focal) {
    this->update();
}

void Camera::move(const vec3 &mov) {
    this->pos += (this->right * mov.x) + (Camera::VERTICAL_UP * mov.y) + (this->focal * mov.z);
}

void Camera::moveTo(const vec3 &_pos) {
    this->pos = _pos;
}

void Camera::eulerAngle(float pitch, float yaw, float roll) {
    float pitchRadians  = radians(pitch);
    float yawRadians    = radians(yaw);
    float xzProj        = cos(pitchRadians);
    this->focal = vec3(xzProj * sin(-yawRadians), sin(pitchRadians), xzProj * cos(-yawRadians));
    this->update();
}

const glm::vec3& Camera::getPos() const {
    return this->pos;
}

const glm::vec3& Camera::getFocal() const {
    return this->focal;
}

Camera::Camera(const std::tuple<glm::vec3, glm::vec3, glm::vec3> &info): pos(glm::vec3(0)) {
    this->focal = get<0>(info);
    this->right = get<1>(info);
    this->up    = get<2>(info);
}

