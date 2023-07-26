//
// Created by 11739 on 2023/7/2.
//

#include <functional>
#include "util/render_util.h"

void ModelInfo::addInstance(glm::mat4 transMtx) {
    this->transMatrices.push_back(transMtx);
}

void ModelInfo::addInstance(std::vector<glm::mat4> transMtx) {
    this->transMatrices.reserve(this->transMatrices.size() + transMtx.size());
    this->transMatrices.insert(this->transMatrices.end(), transMtx.begin(), transMtx.end());
}

ModelInfo::ModelInfo(uint id): id(id) {

}


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

ModelInfo ModelManager::createInfo(const std::string& name) {
    return ModelInfo(this->names[name]);
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

void renderShadow(const Model* model, const glm::mat4* transMtx, size_t amount, const DLight& light) {
    const Shader* shader = DepthShader;
    shader->use();

    setupDirectionalLight(shader, light);
    Buffer mtxBuf(GL_ARRAY_BUFFER);
    mtxBuf.putData(amount * SZ_MAT4F, transMtx);
    model->draw(*shader, mtxBuf);
}


void renderPointShadow(const Model* model, const glm::mat4* transMtx, size_t amount, const PLight& light) {
    const Shader* shader = selectCubeShader(SHADOW);
    shader->use();
    setupPointLight(shader, light);
    glm::vec3 eye = light.pos;
    // space matrices
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 1.0f, light.zNear, light.zFar);
    std::vector<glm::mat4> lightSpaceMats;
    lightSpaceMats.push_back(proj * glm::lookAt(eye, eye + glm::vec3( 1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(eye, eye + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(eye, eye + glm::vec3( 0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(eye, eye + glm::vec3( 0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(eye, eye + glm::vec3( 0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)));
    lightSpaceMats.push_back(proj * glm::lookAt(eye, eye + glm::vec3( 0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0)));

    for (int i = 0; i < 6; ++i) {
        shader->uniformMatrix4fv(Shader::CUBE_SPACE_MATRICES[i], lightSpaceMats[i]);
    }
    Buffer mtxBuf(GL_ARRAY_BUFFER);
    mtxBuf.putData(amount * SZ_MAT4F, transMtx);
    model->draw(*shader, mtxBuf);
}


void renderPointShadow(const Model* model, const glm::mat4& transMtx, const PLight& light) {
    renderPointShadow(model, &transMtx, 1, light);
}

void renderPureColor(const Model* model, const glm::mat4* transMtx, size_t amount, glm::vec3 color) {
    const Shader* shader = selectShader(PURE);
    shader->use();

    shader->uniformVec3(Shader::COLOR, color);
    Buffer mtxBuf(GL_ARRAY_BUFFER);
    mtxBuf.putData(amount * SZ_MAT4F, transMtx);
    model->draw(*shader, mtxBuf);
}

void renderGBuffer(const Model *model, const glm::mat4 *transMtx, size_t amount) {
    const Shader* shader = GBufferShader;
    shader->use();

    Buffer mtxBuf(GL_ARRAY_BUFFER);
    mtxBuf.putData(amount * SZ_MAT4F, transMtx);
    model->draw(*shader, mtxBuf);
}
