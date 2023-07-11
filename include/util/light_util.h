//
// Created by HXYP on 2023/4/29.
//

#ifndef ISLAND_LIGHT_UTIL_H
#define ISLAND_LIGHT_UTIL_H
#include "glm/glm.hpp"
#include "shaders.h"

struct PointLight {
    glm::vec3 color = glm::vec3(0.0f);
    glm::vec3 pos = glm::vec3(0.0f);
    float linear = 1;
    float zNear = .1;
    float zFar = 25.0;
    GLuint shadow = 0;
} const EMPTY_POINT_LIGHT;

struct DirectionalLight {
    glm::vec3 color = glm::vec3(0.0f);
    glm::vec3 injection = glm::vec3(1.0f, 0, 0);
    glm::vec3 ambient = glm::vec3(0.1);
    GLuint shadow = 0;
} const EMPTY_DIRECTIONAL_LIGHT;


void setupPointLight(const Shader* shader, const PointLight& light);
void setupDirectionalLight(const Shader* shader, const DirectionalLight& light);
void setupLight(const Shader* shader, const PointLight& pLight, const DirectionalLight& dLight);

#endif //ISLAND_LIGHT_UTIL_H
