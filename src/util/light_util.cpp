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

    glActiveTexture(GL_TEXTURE0 + POINT_SHADOW_MAP0 + idx);
    glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadow.id);
    shader->uniformInt(header + "depthTex", POINT_SHADOW_MAP0 + idx);
}

void setupPointLight(const Shader* shader, const PointLight& light) {
    shader->uniformVec3( "pointLight.pos",      light.pos);
    shader->uniformVec3( "pointLight.color",    light.color);
    shader->uniformVec3("pointLight.attenu",    light.attenu);
    shader->uniformVec2("pointLight.zNearFar",  light.zNearFar);

    glActiveTexture(GL_TEXTURE0 + POINT_SHADOW_MAP0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, light.shadow.id);
    shader->uniformInt("pointLight.depthTex", POINT_SHADOW_MAP0);
}


void setupDirectionalLight(const Shader* shader, const DirectionalLight& light) {
    shader->uniformVec3(Shader::DirectLight::INJECTION, light.injection);
    shader->uniformVec3(Shader::DirectLight::COLOR,     light.color);
    shader->uniformVec3(Shader::DirectLight::AMBIENT,   light.ambient);

    auto n = static_cast<GLsizei>(light.shadowMaps.size());
    shader->uniformInt(Shader::DirectLight::CSM_LEVELS, n);
    for (GLsizei i = 0; i < n; ++i) {
        const auto& shadow = light.shadowMaps[i];
        shader->uniformMatrix4fv(Shader::DirectLight::LiSPACE_MATRICES[i], shadow.V2LiSpacePV);
        shader->uniformFloat(Shader::DirectLight::FAR_DEPTHS[i], shadow.farDepth);
        glActiveTexture(GL_TEXTURE0 + CSM_SHADOW_MAP0 + i);
        glBindTexture(GL_TEXTURE_2D, shadow.tex.id);
        shader->uniformInt(Shader::DirectLight::CSM_MAPS[i], CSM_SHADOW_MAP0 + i);
    }
}

void setupDLightNoShadow(const Shader* shader, const DirectionalLight& light) {
    shader->uniformVec3(Shader::DirectLight::INJECTION, light.injection);
    shader->uniformVec3(Shader::DirectLight::COLOR,     light.color);
    shader->uniformVec3(Shader::DirectLight::AMBIENT,   light.ambient);
}

void setupLight(const Shader* shader, const Light& light) {

    setupDirectionalLight(shader, light.dLight);
    for (int i = 0; i < MAX_PLIGHT_AMOUNT; ++i) {
        setupPointLight(shader, light.pLights[i], i);
    }
}

Light::Light(const DirectionalLight& dLight, const std::vector<PointLight*>& pLights) {
    EMPTY_POINT_LIGHT.shadow = textures::NO_POINT_SHADOW;
    this->dLight = dLight;
    int i;
    size_t len = MIN(pLights.size(), MAX_PLIGHT_AMOUNT);
    for (i = 0; i < len; ++i) {
        this->pLights[i] = *pLights[i];
    }
    for (; i < MAX_PLIGHT_AMOUNT; ++i) {
        this->pLights[i] = EMPTY_POINT_LIGHT;
    }
}
