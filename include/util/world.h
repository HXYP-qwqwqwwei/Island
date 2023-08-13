//
// Created by HXYP on 2023/7/19.
//

#ifndef ISLAND_WORLD_H
#define ISLAND_WORLD_H
#include <sstream>
#include <vector>
#include <initializer_list>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <cmath>
#include <limits>
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
void SetupPVMatrix(Camera& camera);
void RenderWorldGBuffer(Camera& camera, RenderType type);
void RenderShadow(Camera& camera);
uint CreatePointLight(glm::vec3 pos, glm::vec3 color, GLsizei shadowRes, glm::vec3 attenu = {1, 0, 1}, glm::vec2 zNearFar = {0.1, 25.0});
uint CreatePointLightNoShadow(glm::vec3 pos, glm::vec3 color, glm::vec3 attenu = {1, 0, 1});
void SetDirectLight(glm::vec3 injection, glm::vec3 color, GLsizei shadowRes, GLsizei csmLevels, glm::vec3 ambient = glm::vec3(0.01f));
void RenderSSAO(const FrameBuffer& gBuffer, const glm::vec3* samples, size_t n, const Texture2D& texNoise, GLfloat radius = 1.0, GLfloat power = 1.0);
void ProcessGBuffer(const FrameBuffer& gBuffer, const Texture2D& ssao = textures::WHITE_GRAY);
void RenderFrame(const FrameBuffer& frame, std::initializer_list<int> indices);
//void RenderFrame();
//void SetScreenTextures(const std::vector<GLuint>& textures);
//void SetScreenTextures(const std::initializer_list<GLuint>& textures);

void Blur(int index, int blurLevel);


#endif //ISLAND_WORLD_H
