//
// Created by HXYP on 2023/3/16.
//

#ifndef ISLAND_CAMERA_H
#define ISLAND_CAMERA_H
#include <tuple>
#include "glm/gtc/matrix_transform.hpp"
#include "game_util.h"
#include "shaders.h"


class Camera {
private:
    static const glm::vec3 VERTICAL_UP;
    glm::vec3 pos    = glm::vec3(0, 0, 0);
    glm::vec3 focal  = glm::vec3(0, 0, 1);
    glm::vec3 up     = glm::vec3(0, 1, 0);
    glm::vec3 right  = glm::vec3(1, 0, 0);

    GLfloat zNear;
    GLfloat zFar;

    void update();

public:
    explicit Camera(const glm::vec3& pos = {0, 0, 0}, const glm::vec3& focal = {0, 0, 1}, GLfloat zNear = 0.1, GLfloat zFar = 100.0);
    explicit Camera(const std::tuple<glm::vec3, glm::vec3, glm::vec3>& info, GLfloat zNear = 0.1, GLfloat zFar = 100.0);
    void move(const glm::vec3& mov);
    void moveTo(const glm::vec3& pos);
    void eulerAngle(float pitch, float yaw, float roll = 0);
    glm::mat4 getView();
    [[nodiscard]] GLfloat near() const;
    [[nodiscard]] GLfloat far() const;
    [[nodiscard]] const glm::vec3& getPos() const;
    [[nodiscard]] const glm::vec3& getFocal() const;
};


#endif //ISLAND_CAMERA_H
