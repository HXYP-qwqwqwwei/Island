//
// Created by HXYP on 2023/4/29.
//

#include "util/light_util.h"

PointLight EMPTY_POINT_LIGHT;
DirectionalLight EMPTY_DIRECTIONAL_LIGHT;

void setupPointLight(const Shader* shader, const PointLight& light, int idx) {
    std::string header = "pointLights[";
    header += std::to_string(idx) + "].";
    shader->uniformVec3(header + "pos",         light.pos);
    shader->uniformVec3(header + "color",       light.color);
    shader->uniformVec3(header + "attenu",      light.attenu);
    shader->uniformVec2(header + "zNearFar",    light.zNearFar);

    glActiveTexture(GL_TEXTURE16 + idx);
    glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadow);
    shader->uniformInt(header + "depthTex", 16 + idx);
}

void setupPointLight(const Shader* shader, const PointLight& light) {
    shader->uniformVec3( "pointLight.pos",      light.pos);
    shader->uniformVec3( "pointLight.color",    light.color);
    shader->uniformVec3("pointLight.attenu",    light.attenu);
    shader->uniformVec2("pointLight.zNearFar",  light.zNearFar);

    glActiveTexture(GL_TEXTURE16);
    glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadow);
    shader->uniformInt("pointLight.depthTex", 16);
}


void setupDirectionalLight(const Shader* shader, const DirectionalLight& light) {
    shader->uniformVec3("directLight.injection", light.injection);
    shader->uniformVec3("directLight.color", light.color);
    shader->uniformVec3("directLight.ambient", light.ambient);
    shader->uniformMatrix4fv(Shader::LIGHT_SPACE_MATRIX, light.spaceMtx);

    glActiveTexture(GL_TEXTURE30);
    glBindTexture(GL_TEXTURE_2D, light.shadow);
    shader->uniformInt("directLight.depthTex", 30);
}

void setupLight(const Shader* shader, const Light& light) {

    setupDirectionalLight(shader, light.dLight);
    for (int i = 0; i < MAX_PLIGHT_AMOUNT; ++i) {
        setupPointLight(shader, light.pLights[i], i);
    }
}

Light::Light(const DirectionalLight& dLight, const std::vector<PointLight*>& pLights) {
    this->dLight = dLight;
    int i;
    size_t len = min(pLights.size(), MAX_PLIGHT_AMOUNT);
    for (i = 0; i < len; ++i) {
        this->pLights[i] = *pLights[i];
    }
    for (; i < MAX_PLIGHT_AMOUNT; ++i) {
        this->pLights[i] = EMPTY_POINT_LIGHT;
    }
}
