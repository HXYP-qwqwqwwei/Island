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
using DLight = DirectionalLight;


class ModelManager {
public:
    void put(const std::function<Model()> &creator, const std::string &name);
    const Model* getModel(const std::string& name);
private:
    const Model* operator[](size_t i);
    std::vector<Model> models;
    std::unordered_map<std::string, uint> names;
};

void render(const Model* model, RenderType type, const Camera& camera, const glm::mat4* transMtx, size_t amount,
            const PLight& pLight, const DLight& dLight);

void render(const Model* model, RenderType type, const Camera& camera, const glm::mat4& transMtx,
            const PLight& pLight, const DLight& dLight);

void renderCube(const Model* model, RenderType type, const glm::vec3& center, const glm::mat4* transMtx, size_t amount,
                const PLight& pLight, const DLight& dLight);

void renderCube(const Model* model, RenderType type, const glm::vec3& center, const glm::mat4& transMtx,
                const PLight& pLight, const DLight& dLight);


#endif //ISLAND_MODEL_MANAGER_H
