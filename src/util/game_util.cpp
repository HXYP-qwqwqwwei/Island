//
// Created by HXYP on 2023/3/17.
//

#include "util/game_util.h"

GLsizei GameScrWidth = 1000;
GLsizei GameScrHeight = 1000;

glm::vec3 initPos = glm::vec3(0, 0, 3);
glm::vec3 cameraMov = glm::vec3(0, 0, 0);
float gameSPF = 0;
float playerPitch;
float playerYaw;


#define SCR_CENTER_X (static_cast<double>(GameScrWidth) / 2)
#define SCR_CENTER_Y (static_cast<double>(GameScrHeight) / 2)

GLFWwindow* createWindow(GLsizei width, GLsizei height) {
    GameScrWidth = width;
    GameScrHeight = height;
    GLFWwindow* window = glfwCreateWindow(GameScrWidth, GameScrHeight, "Island", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    // 为窗口设置回调函数，告诉OpenGL在窗口大小变化时需要做什么
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    resetCursor(window);
    return window;
}


void resetCursor(GLFWwindow* window) {
    glfwSetCursorPos(window, SCR_CENTER_X, SCR_CENTER_Y);
}


void processInput(GLFWwindow* window) {
    cameraMov = glm::vec3(0, 0, 0);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    float speed = 8;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraMov -= CoordAxis::ZP;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraMov += CoordAxis::ZP;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraMov -= CoordAxis::XP;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraMov += CoordAxis::XP;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraMov -= CoordAxis::YP;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraMov += CoordAxis::YP;
    float len = glm::length(cameraMov);
    if (len >= 1e-7f) {
        cameraMov = (gameSPF * speed / len) * cameraMov;
    }
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    GameScrWidth = width;
    GameScrHeight = height;
}


void cursor_pos_callback(GLFWwindow* window, double x, double y) {
    double dx = x - SCR_CENTER_X;
    double dy = y - SCR_CENTER_Y;
    playerYaw   += static_cast<float>(dx * 0.02);
    playerPitch += static_cast<float>(dy * 0.02);
    if (playerPitch >= 89.5) {
        playerPitch = 89.5;
    } else if (playerPitch <= -89.5) {
        playerPitch = -89.5;
    }
    glfwSetCursorPos(window, SCR_CENTER_X, SCR_CENTER_Y);
}

