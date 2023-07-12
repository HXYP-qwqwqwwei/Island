//
// Created by HXYP on 2023/4/29.
//

#include "util/light_util.h"

void setupPointLight(const Shader* shader, const PointLight& light) {
    shader->uniformVec3("pointLightPosition", light.pos);
    shader->uniformVec3("pointLight.color", light.color);
    shader->uniformFloat("pointLight.linear", light.linear);
    shader->uniformFloat("pointLight.zFar", light.zFar);

    glActiveTexture(GL_TEXTURE31);
    glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadow);
    shader->uniformInt("pointLight.shadowMap", 31);
}

void setupDirectionalLight(const Shader* shader, const DirectionalLight& light) {
    shader->uniformVec3("directLightInjection", light.injection);
    shader->uniformVec3("directLight.color", light.color);
    shader->uniformVec3("directLight.ambient", light.ambient);

    glActiveTexture(GL_TEXTURE30);
    glBindTexture(GL_TEXTURE_2D, light.shadow);
    shader->uniformInt("directLight.shadowMap", 30);
}

void setupLight(const Shader* shader, const PointLight& pLight, const DirectionalLight& dLight) {
    setupPointLight(shader, pLight);
    setupDirectionalLight(shader, dLight);
}
