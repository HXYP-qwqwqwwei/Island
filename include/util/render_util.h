//
// Created by 11739 on 2023/7/2.
//

#ifndef ISLAND_MODEL_MANAGER_H
#define ISLAND_MODEL_MANAGER_H
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include "shaders.h"
#include "light_util.h"
#include "Model.h"
#include "BuiltinMesh.h"
#include "Screen.h"

using PLight = PointLight;
using DLight = DirectionalLight;
class ModelManager;

struct TransWithColor {
    glm::mat4 trans;
    glm::vec3 color;
};

struct ModelInfo {
    uint id;
    std::vector<glm::mat4> transMatrices;
    void addInstance(glm::mat4 transMtx);
    void addInstance(std::vector<glm::mat4> transMtx);
private:
    friend ModelManager;
    explicit ModelInfo(uint id);
};

class ModelManager {
public:
    void put(const std::function<Model()> &creator, const std::string &name);
    const Model* getModel(const std::string& name);
    ModelInfo createInfo(const std::string& name);
    const Model* operator[](size_t i);
private:
    std::vector<Model> models;
    std::unordered_map<std::string, uint> names;
};

void render(const Model* model, RenderType type, const Camera& camera, const glm::mat4* transMtx, size_t amount, const Light& light);

void render(const Model* model, RenderType type, const Camera& camera, const glm::mat4& transMtx, const Light& light);

void renderShadow(const Model* model, const glm::mat4* transMtx, size_t amount, const DLight& light);

void renderPointShadow(const Model* model, const glm::mat4* transMtx, size_t amount, const PLight& light);

void renderPointShadow(const Model* model, const glm::mat4& transMtx, const PLight& light);

void renderPureColor(const Model* model, const TransWithColor* trans, size_t amount);

void renderLightModels(const Model* model, PLight const* const* lights, size_t amount);

void renderGBuffer(const Model *model, RenderType type, const Camera& camera, const glm::mat4 *transMtx, size_t amount);

void lightGBuffer(Screen *gScreen, const DLight &light);

void lightGBuffer(const Mesh *mesh, const PLight &light);

void lightGBufferNoShadow(const Mesh *mesh, PLight const *const *lights, size_t amount);

#endif //ISLAND_MODEL_MANAGER_H
