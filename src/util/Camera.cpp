//
// Created by HXYP on 2023/3/16.
//

#include "util/Camera.h"
using namespace glm;

const vec3 Camera::VERTICAL_UP = CoordAxis::YP;

mat4 Camera::getView() {
    glm::vec3 eye   = this->pos;
    return {
            glm::vec4(this->right.x, this->up.x, this->focal.x, 0),
            glm::vec4(this->right.y, this->up.y, this->focal.y, 0),
            glm::vec4(this->right.z, this->up.z, this->focal.z, 0),
            glm::vec4(-glm::dot(right, eye), -glm::dot(up, eye), -glm::dot(this->focal, eye), 1)
    };
}

void Camera::update() {
    if (this->focal.x == 0 && this->focal.z == 0) {
        this->focal.y = (this->focal.y >= 0) ? 1.0 : -1.0;
        this->right = glm::vec3(1.0, 0.0, 0.0);
    } else {
        this->right = glm::normalize(glm::cross(VERTICAL_UP, this->focal));
    }
    this->up = glm::cross(this->focal, right);
}

Camera::Camera(const vec3 &pos, const vec3& focal, float zNear, float zFar): pos(pos), focal(focal), zNear(zNear), zFar(zFar) {
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

Camera::Camera(const std::tuple<glm::vec3, glm::vec3, glm::vec3> &info, float zNear, float zFar): pos(glm::vec3(0)), zNear(zNear), zFar(zFar) {
    this->focal = get<0>(info);
    this->right = get<1>(info);
    this->up    = get<2>(info);
}

GLfloat Camera::near() const {
    return zNear;
}

GLfloat Camera::far() const {
    return zFar;
}

