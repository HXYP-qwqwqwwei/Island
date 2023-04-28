//
// Created by HXYP on 2023/3/17.
//

#ifndef ISLAND_GAME_UTIL_H
#define ISLAND_GAME_UTIL_H
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include <iostream>
#include "Camera.h"

struct CoordAxis {
    constexpr static glm::vec3 XP = glm::vec3(1.0f, 0.0f, 0.0f);
    constexpr static glm::vec3 YP = glm::vec3(0.0f, 1.0f, 0.0f);
    constexpr static glm::vec3 ZP = glm::vec3(0.0f, 0.0f, 1.0f);
};


extern GLsizei gameScrWidth;
extern GLsizei gameScrHeight;
extern glm::vec3 initPos;
extern glm::vec3 cameraMov;
extern float gameSPF;
extern float playerPitch;
extern float playerYaw;

GLFWwindow* createWindow(GLsizei width = 1000, GLsizei height = 1000);
void resetCursor(GLFWwindow* window);

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_pos_callback(GLFWwindow* window, double x, double y);

#endif //ISLAND_GAME_UTIL_H
