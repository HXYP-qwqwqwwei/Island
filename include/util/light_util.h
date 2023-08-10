//
// Created by HXYP on 2023/4/29.
//

#ifndef ISLAND_LIGHT_UTIL_H
#define ISLAND_LIGHT_UTIL_H
#include <vector>
#include "glm/glm.hpp"
#include "shaders.h"
#include "defs.h"
#include "buffer_util.h"
#include "texture_util.h"

#define MAX_PLIGHT_AMOUNT 4

struct PointLight {
    glm::vec3 color     = glm::vec3(0.0f);
    glm::vec3 pos       = glm::vec3(0.0f);
    glm::vec3 attenu    = glm::vec3(1.0, 0.0, 1.0);    // attenuation coefficients
    glm::vec2 zNearFar  = glm::vec2(0.1f, 25.0f);
    TextureCube shadow;
};

struct DirectionalLight {
    glm::vec3 color = glm::vec3(0.0f);
    glm::vec3 injection = glm::vec3(1.0f, 0, 0);
    glm::vec3 ambient = glm::vec3(0.1f);
    glm::mat4 spaceMtx = glm::mat4(1.0f);
    TextureCube shadow;
};

extern PointLight EMPTY_POINT_LIGHT;
extern DirectionalLight EMPTY_DIRECTIONAL_LIGHT;


struct Light {
    DirectionalLight dLight;
    PointLight pLights[MAX_PLIGHT_AMOUNT];

    Light(const DirectionalLight& dLight, const std::vector<PointLight*>& pLights);
};


void setupPointLight(const Shader* shader, const PointLight& light, int idx);
void setupPointLight(const Shader* shader, const PointLight& light);
void setupDirectionalLight(const Shader* shader, const DirectionalLight& light);
void setupLight(const Shader* shader, const Light& light);

#endif //ISLAND_LIGHT_UTIL_H
