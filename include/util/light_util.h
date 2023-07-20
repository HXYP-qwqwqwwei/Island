//
// Created by HXYP on 2023/4/29.
//

#ifndef ISLAND_LIGHT_UTIL_H
#define ISLAND_LIGHT_UTIL_H
#include <vector>
#include "glm/glm.hpp"
#include "shaders.h"
#include "defs.h"

#define MAX_PLIGHT_AMOUNT 4

struct PointLight {
    glm::vec3 color = glm::vec3(0.0f);
    glm::vec3 pos = glm::vec3(0.0f);
    float linear = 1;
    float zNear = .1;
    float zFar = 25.0;
    GLuint shadow;
};

struct DirectionalLight {
    glm::vec3 color = glm::vec3(0.0f);
    glm::vec3 injection = glm::vec3(1.0f, 0, 0);
    glm::vec3 ambient = glm::vec3(0.1);
    GLuint shadow;
};

extern PointLight EMPTY_POINT_LIGHT;
extern DirectionalLight EMPTY_DIRECTIONAL_LIGHT;


struct Light {
    DirectionalLight dLight;
    PointLight pLights[MAX_PLIGHT_AMOUNT];

    Light(DirectionalLight dLight, std::vector<PointLight> pLights);
};


void setupPointLight(const Shader* shader, const PointLight& light, int idx);
void setupPointLight(const Shader* shader, const PointLight& light);
void setupDirectionalLight(const Shader* shader, const DirectionalLight& light);
void setupLight(const Shader* shader, const Light& light);

#endif //ISLAND_LIGHT_UTIL_H
