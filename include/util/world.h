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
#include "glm/glm.hpp"

extern ModelManager MODEL_MANAGER;


void InitWorld();
void PutModelInfo(RenderType type, const ModelInfo* modelInfo);
void Flush();
void RenderWorld(Camera& camera, FrameBuffer& frame);
void RenderShadow();
uint CreatePointLight(glm::vec3 pos, glm::vec3 color, GLsizei shadowRes, float linear = 1.0f, float zNear = 0.1f, float zFar = 25.0f);
void SetDirectLight(glm::vec3 injection, glm::vec3 color, GLsizei shadowRes, glm::vec3 ambient = glm::vec3(0.01f));

#endif //ISLAND_WORLD_H
