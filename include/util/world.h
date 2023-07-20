//
// Created by HXYP on 2023/7/19.
//

#ifndef ISLAND_WORLD_H
#define ISLAND_WORLD_H
#include <vector>
#include "Model.h"
#include "render_util.h"

struct ModelInfo {
    int id;
    size_t amount;
    glm::mat4* trans;
};


void InitWorld();

#endif //ISLAND_WORLD_H
