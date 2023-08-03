//
// Created by HXYP on 2023/7/19.
//

#ifndef ISLAND_WORLD_H
#define ISLAND_WORLD_H
#include <vector>
#include <initializer_list>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include "Model.h"
#include "render_util.h"
#include "Screen.h"
#include "light_util.h"
#include "game_util.h"
#include "glm/glm.hpp"
#include "shapes.h"

extern ModelManager MODEL_MANAGER;


void InitWorld();
void PutModelInfo(RenderType type, const ModelInfo* modelInfo);
void Flush();
void BindFrameBuffer(FrameBuffer* frame);
void ClearBuffer(int bits);
void ClearBuffer(int bits, float r, float g, float b);
void RenderWorld(Camera& camera, FrameBuffer& frame);
void EnableDepthTest(GLenum func = GL_LESS);
void DisableDepthTest();

void EnableStencilTest(GLenum sFail);
void DisableStencilTest();

void RenderModelsInWorld(Camera& camera, RenderType type);
void SetupPVMatrix(Camera& camera, float zNear = 0.1f, float zFar = 100.0f);
void RenderWorldGBuffer(Camera& camera, RenderType type);
void RenderShadow();
uint CreatePointLight(glm::vec3 pos, glm::vec3 color, GLsizei shadowRes, glm::vec3 attenu = {1, 0, 1}, glm::vec2 zNearFar = {0.1, 25.0});
uint CreatePointLightNoShadow(glm::vec3 pos, glm::vec3 color, glm::vec3 attenu = {1, 0, 1});
void SetDirectLight(glm::vec3 injection, glm::vec3 color, GLsizei shadowRes, glm::vec3 ambient = glm::vec3(0.01f));
void ProcessGBuffer(const Camera& camera, const FrameBuffer& gBuffer);
void RenderFrame(const FrameBuffer& frame, std::initializer_list<int> indices);
//void RenderFrame();
//void SetScreenTextures(const std::vector<GLuint>& textures);
//void SetScreenTextures(const std::initializer_list<GLuint>& textures);

void Blur(int index, int blurLevel);


#endif //ISLAND_WORLD_H
