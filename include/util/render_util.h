//
// Created by 11739 on 2023/7/2.
//

#ifndef ISLAND_MODEL_MANAGER_H
#define ISLAND_MODEL_MANAGER_H
#include <vector>
#include <unordered_map>
#include <string>
#include "shaders.h"
#include "light_util.h"
#include "Model.h"

using PLight = PointLight;
class ModelManager;

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

void renderPointShadow(const Model* model, const glm::mat4* transMtx, size_t amount, const PLight& light);

void renderPointShadow(const Model* model, const glm::mat4& transMtx, const PLight& light);

void renderPureColor(const Model* model, const glm::mat4* transMtx, size_t amount, glm::vec3 color);


#endif //ISLAND_MODEL_MANAGER_H
