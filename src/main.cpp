#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "util/Shader.h"
#include "util/stb_image.h"
#include "util/game_util.h"
#include "util/shapes.h"
#include "util/Model.h"
#include "util/texture_util.h"
#include "util/Screen.h"
#include "util/buffer_util.h"

using GLObject = GLuint;
using GLLoc = GLint;

int main() {
    glfwInit();
    // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    // 核心模式，另外，GLFW_OPENGL_COMPAT_PROFILE是兼容模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 新建窗口对象
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow* window = createWindow(1920, 1080);

    // 在利用GLAD调用任何OpenGL函数前，需要先对GLAD初始化
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }


    // Shader program
    Shader solidShader, simpleShader, transparentShader, cutoutShader, screenShader, skyShader;
    solidShader.loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    solidShader.loadShader("SolidShader.frag", GL_FRAGMENT_SHADER);
    solidShader.link();

    simpleShader.loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    simpleShader.loadShader("SimpleShader.frag", GL_FRAGMENT_SHADER);
    simpleShader.link();

    transparentShader.loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    transparentShader.loadShader("TransparentShader.frag", GL_FRAGMENT_SHADER);
    transparentShader.link();

    cutoutShader.loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    cutoutShader.loadShader("TransparentShader.frag", GL_FRAGMENT_SHADER);
    cutoutShader.link();

    screenShader.loadShader("ScreenShader.vert", GL_VERTEX_SHADER);
    screenShader.loadShader("ScreenShader.frag", GL_FRAGMENT_SHADER);
    screenShader.link();

    skyShader.loadShader("SkyShader.vert", GL_VERTEX_SHADER);
    skyShader.loadShader("SkyShader.frag", GL_FRAGMENT_SHADER);
    skyShader.link();


    // 在开始渲染前，需要告诉OpenGL窗口的坐标和大小
    // 因为OpenGL使用的任何坐标范围是[-1, +1]，通过指定大小后就可以将其映射到[0, W]和[0, H]
    // 例如(-0.5, 0.5)将会被映射到屏幕的(W/4, 3H/4)
    glViewport(0, 0, gameScrWidth, gameScrHeight);


    double t0;
    Camera camera(initPos);

    // Hide cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Models
    Model nanoSuitModel("resources/nanosuit/nanosuit.obj");
    Mesh* cube      = shapes::Cube(1);
    Mesh* lightCube = shapes::Cube(0.2f);
    Mesh* ground    = shapes::Rectangle(10, 10, 1, 1);
    Mesh* rgbWindow = shapes::Rectangle(1, 1);
    Mesh* grass     = shapes::Rectangle(1, 1);


    const std::string dir = "resources/textures";
    Texture2D cubeDiff = load_texture("container2.png", dir, aiTextureType_DIFFUSE);
    Texture2D cubeSpec = load_texture("container2_specular.png", dir, aiTextureType_SPECULAR);
    Texture2D groundDiff = load_texture("plank_flooring_diff_1k.jpg", dir, aiTextureType_DIFFUSE);
    Texture2D groundSpec = load_texture("plank_flooring_rough_1k.jpg", dir, aiTextureType_SPECULAR);
    Texture2D grassDiff = load_texture("grass.png", dir, aiTextureType_DIFFUSE, GL_CLAMP_TO_EDGE);
    Texture2D emptySpec = load_texture("empty.png", dir, aiTextureType_SPECULAR);
    Texture2D windowTexDiff = load_texture("window_transparent.png", dir, aiTextureType_DIFFUSE);

    GLuint skyBoxTex = load_cube_map(
            {"skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg"},
            dir
    );

    GLuint emptyEnvMap = load_cube_map(
            {"empty.png", "empty.png", "empty.png", "empty.png", "empty.png", "empty.png"},
            dir
    );


    cube->addTexture(cubeDiff).addTexture(cubeSpec);
    ground->addTexture(groundDiff).addTexture(groundSpec);
    grass->addTexture(grassDiff).addTexture(emptySpec);
    rgbWindow->addTexture(windowTexDiff).addTexture(emptySpec);

    SkyBox* skyBox  = shapes::SkyBoxCube(skyBoxTex);

    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendEquation(GL_FUNC_ADD);   // default

    FrameBuffer frameBuffer(gameScrWidth, gameScrHeight, COLOR | DEPTH | STENCIL);
    Screen* screen  = shapes::ScreenRect({frameBuffer.getTexture()});

    FrameBuffer shadowMap(1024, 1024, DEPTH);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        // ...

        // rendering
        double t1 = glfwGetTime();
        gameSPF = static_cast<float>(t1 - t0);
        t0 = t1;

        frameBuffer.bind();

        // 每当glClear被调用，color buffer中的颜色都会被替换成glClearColor中配置的颜色
        glClearColor(0.1, 0.1, 0.1, 1.0F);
        // Depth test
//        glDepthMask(GL_FALSE);  // Read-Only
        glDepthFunc(GL_LESS);   // Default
        // Stencil test
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  // when a fragment passed ST, replace its value
        // Blend
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // 清除上一帧的颜色/深度测试/模板测试缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

//        glm::vec3 pLight(1, 1, 1);
        glm::vec3 pLight(1.0f);
        glm::vec3 dLight(.0f);
        glm::vec3 dLightAmbient(.1f);
        glm::vec3 dLightInjection(1, -2, 1);
        float pLightLinear = 0.01;
        float pLightQuadratic = 0.02;
//        glm::vec3 dLight(1.0f, 1.0f, 1.0f);
//        glm::vec3 pLightPos(2*sin(t1), -1, 2*cos(t1));
        glm::vec3 pLightPos(1, 0, 2);
        // camera
        camera.move(cameraMov);
        camera.eulerAngle(playerPitch, playerYaw);
        const glm::mat4 view = camera.getView();
        const glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) gameScrWidth / (float)gameScrHeight, 0.1f, 100.0f);
        glm::mat4 model(1.0f);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        // Solid
        solidShader.use();
        solidShader.uniformVec3("pointLight.color", pLight);
        solidShader.uniformVec3("pointLight.pos", pLightPos);
        solidShader.uniformFloat("pointLight.linear", pLightLinear);
        solidShader.uniformFloat("pointLight.quadratic", pLightQuadratic);
        solidShader.uniformVec3("directLight.color", dLight);
        solidShader.uniformVec3("directLight.injection", dLightInjection);
        solidShader.uniformVec3("directLight.ambient", dLightAmbient);
        solidShader.uniformVec3("viewPos", camera.getPos());
        solidShader.uniformMatrix4fv("proj", proj);
        solidShader.uniformMatrix4fv("view", view);


//        solidShader.uniformVec3("spotLight.color", pLight);
//        solidShader.uniformVec3("spotLight.pos", camera.getPos());
//        solidShader.uniformVec3("spotLight.direction", camera.getFocal());
//        solidShader.uniformFloat("spotLight.cutOff", glm::cos(glm::radians(15.0f)));


        solidShader.uniformMatrix4fv("model", model);

        // open stencil test
//        glStencilFunc(GL_ALWAYS, 1, 0xFF);
//        glStencilMask(0xFF);

        cube->setEnvironmentMap(emptyEnvMap);
//        nanoSuitModel.Draw(solidShader);
        cube->draw(solidShader);

        // close stencil test
//        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);  // whe
//        glStencilMask(0x00);    // Disable writing

        model = glm::translate(model, glm::vec3(-1, 0, -2));
        solidShader.uniformMatrix4fv("model", model);
        cube->draw(solidShader);

        // ground
        solidShader.use();
        solidShader.uniformFloat("material.shininess", 2.0f);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.501, 0));
        model = glm::rotate(model, glm::radians(-90.0f), CoordAxis::XP);
        solidShader.uniformMatrix4fv("model", model);
        ground->setEnvironmentMap(emptyEnvMap);
        ground->draw(solidShader);

        // pLight
        simpleShader.use();
        simpleShader.uniformMatrix4fv("proj", proj);
        simpleShader.uniformMatrix4fv("view", view);
        simpleShader.uniformVec3("color", glm::vec3(1, 1, 1));
        model = glm::mat4(1.0f);
        model = glm::translate(model, pLightPos);
        simpleShader.uniformMatrix4fv("model", model);
        lightCube->draw(simpleShader);


        // Sky box
//        skyShader.use();
//        skyShader.uniformMatrix4fv("proj", proj);
//        skyShader.uniformMatrix4fv("view", glm::mat4(glm::mat3(view)));
//        skyBox->draw(skyShader);


        // Cutout
        cutoutShader.use();
        cutoutShader.uniformVec3("pointLight.color", pLight);
        cutoutShader.uniformVec3("pointLight.pos", pLightPos);
        cutoutShader.uniformFloat("pointLight.linear", pLightLinear);
        cutoutShader.uniformFloat("pointLight.quadratic", pLightQuadratic);
        cutoutShader.uniformVec3("directLight.color", dLight);
        cutoutShader.uniformVec3("directLight.injection", dLightInjection);
        cutoutShader.uniformVec3("directLight.ambient", dLightAmbient);
        cutoutShader.uniformVec3("viewPos", camera.getPos());
        cutoutShader.uniformMatrix4fv("proj", proj);
        cutoutShader.uniformMatrix4fv("view", view);

        // two grass
        model = glm::translate(glm::mat4(1), glm::vec3(-1, 0, 0.5f));
        cutoutShader.uniformMatrix4fv("model", model);
        grass->draw(cutoutShader);

        model = glm::translate(model, glm::vec3(2, 0, 0));
        cutoutShader.uniformMatrix4fv("model", model);
        grass->draw(cutoutShader);


        // draw outline (result from stencil test)
//        glDisable(GL_DEPTH_TEST);
//        simpleShader.use();
//        simpleShader.uniformVec3("viewPos", camera.getPos());
//        simpleShader.uniformMatrix4fv("proj", proj);
//        simpleShader.uniformMatrix4fv("view", view);
//
//        model = glm::scale(glm::mat4(1), glm::vec3(1.1));
//        simpleShader.uniformMatrix4fv("model", model);
//        simpleShader.uniformVec3("color", glm::vec3(1, 0, 1));
//        cube->draw(simpleShader);
//        glStencilMask(0xFF);
//        glEnable(GL_DEPTH_TEST);


        // Transparent: two rgbWindows
        transparentShader.use();
        transparentShader.uniformVec3("pointLight.color", pLight);
        transparentShader.uniformVec3("pointLight.pos", pLightPos);
        transparentShader.uniformFloat("pointLight.linear", pLightLinear);
        transparentShader.uniformFloat("pointLight.quadratic", pLightQuadratic);
        transparentShader.uniformVec3("directLight.color", dLight);
        transparentShader.uniformVec3("directLight.injection", dLightInjection);
        transparentShader.uniformVec3("directLight.ambient", dLightAmbient);
        transparentShader.uniformVec3("viewPos", camera.getPos());
        transparentShader.uniformMatrix4fv("proj", proj);
        transparentShader.uniformMatrix4fv("view", view);

        glm::vec3 poses[3];
        poses[0] = glm::vec3(-2, 0, 2.5f);
        poses[1] = glm::vec3(1, 0, 1.5f);
        poses[2] = glm::vec3(-3, 0, 1.5f);
        auto cmp = [&] (const glm::vec3& v1, glm::vec3& v2) {
            return glm::distance(camera.getPos(), v1) > glm::distance(camera.getPos(), v2);
        };
        std::sort(poses, poses+3, cmp);
        for (const auto& pos : poses) {
            glm::mat4 m(1);
            transparentShader.uniformMatrix4fv("model", glm::translate(m, pos));
            rgbWindow->draw(transparentShader);
        }


        // Post-Production
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        screenShader.use();
        screen->draw(screenShader);

        glfwSwapBuffers(window);
        // 检查所有事件并更新窗口状态，否则窗口将无法响应外部输入（包括鼠标和键盘）
        glfwPollEvents();

        // 双缓冲技术：在按帧绘制的时候，如果使用单一缓冲，用户将看到绘制过程，有可能会导致屏幕闪烁
        // 因此要使用两个缓冲区来保存数据，前缓冲用于显示，后缓冲用于绘制，因而在每一帧开始时，需要交换将两个缓冲区交换
        processInput(window);
    }

    glfwTerminate();
    return 0;
}


