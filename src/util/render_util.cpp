//
// Created by 11739 on 2023/7/2.
//

#include <functional>
#include "util/render_util.h"

void ModelManager::put(const std::function<Model()> &creator, const std::string &name) {
    uint id = this->models.size();
    this->models.emplace_back(creator());
    this->names[name] = id;
}

const Model* ModelManager::getModel(const std::string &name) {
    auto it = names.find(name);
    if (it != names.end()) {
        return &this->models[it->second];
    }
    std::cout << "ERROR::ModelManager::Model \"" << name << "\" not found\n";
    return &this->models[0];
}

const Model *ModelManager::operator[](size_t i) {
    return &this->models[i];
}


void render(const Model* model, RenderType type, const Camera& camera, const glm::mat4* transMtx, size_t amount, const Light& light) {
    const Shader* shader = selectShader(type);
    shader->use();

    setupLight(shader, light);
    shader->uniformVec3(Shader::VIEW_POS, camera.getPos());
    shader->uniformFloat(Shader::SHININESS, 2.0f);
    Buffer mtxBuf(GL_ARRAY_BUFFER);
    mtxBuf.putData(amount * SZ_MAT4F, transMtx);
    model->draw(*shader, mtxBuf);
}

void render(const Model* model, RenderType type, const Camera& camera, const glm::mat4& transMtx, const Light& light) {
    render(model, type, camera, &transMtx, 1, light);
}


void renderPointShadow(const Model* model, RenderType type, const glm::vec3& center, const glm::mat4* transMtx, size_t amount, const PLight& light) {
    const Shader* shader = selectCubeShader(type);
    shader->use();
    setupPointLight(shader, light);
    // space matrices
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, light.zNear, light.zFar);
    std::vector<glm::mat4> lightSpaceMats;
    lightSpaceMats.push_back(proj * glm::lookAt(center, center + glm::vec3( 1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(center, center + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(center, center + glm::vec3( 0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(center, center + glm::vec3( 0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(center, center + glm::vec3( 0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(center, center + glm::vec3( 0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0)));

    for (int i = 0; i < 6; ++i) {
        shader->uniformMatrix4fv(Shader::CUBE_SPACE_MATRICES[i], lightSpaceMats[i]);
    }
    Buffer mtxBuf(GL_ARRAY_BUFFER);
    mtxBuf.putData(amount * SZ_MAT4F, transMtx);
    model->draw(*shader, mtxBuf);
}


void renderPointShadow(const Model* model, RenderType type, const glm::vec3& center, const glm::mat4& transMtx, const PLight& light) {
    renderPointShadow(model, type, center, &transMtx, 1, light);
}


//ModelInfo::ModelInfo(Model model, RenderType type, uint id) : model(std::move(model)), type(type), id(id) {
//
//}
