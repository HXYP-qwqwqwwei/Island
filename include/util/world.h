//
// Created by HXYP on 2023/7/19.
//

#ifndef ISLAND_WORLD_H
#define ISLAND_WORLD_H
#include <vector>
#include <initializer_list>
#include <unordered_map>
#include "Model.h"
#include "render_util.h"
#include "light_util.h"

struct ModelInfo {
    int id;
    std::vector<glm::mat4> transMatrices;

    void addInstance(glm::mat4 transMtx);
    void addInstance(std::initializer_list<glm::mat4> transMtx);
};


void InitWorld();
void putModelInfo(RenderType type, const ModelInfo* modelInfo);
void RenderWorld();
void RenderWorldCube();

#endif //ISLAND_WORLD_H
