#include <iostream>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "util/shaders.h"
#include "util/stb_image.h"
#include "util/game_util.h"
#include "util/shapes.h"
#include "util/Model.h"
#include "util/texture_util.h"
#include "util/Screen.h"
#include "util/buffer_util.h"
#include "util/light_util.h"
#include "util/ModelManager.h"

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

    // compile Shader programs
    compileShaders();

    // 在开始渲染前，需要告诉OpenGL窗口的坐标和大小
    // 因为OpenGL使用的任何坐标范围是[-1, +1]，通过指定大小后就可以将其映射到[0, W]和[0, H]
    // 例如(-0.5, 0.5)将会被映射到屏幕的(W/4, 3H/4)
    glViewport(0, 0, gameScrWidth, gameScrHeight);


    // Hide cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    const std::string dir = "resources/textures";
    Texture2D cubeDiff = load_texture("container2.png", dir, aiTextureType_DIFFUSE);
    Texture2D cubeSpec = load_texture("container2_specular.png", dir, aiTextureType_SPECULAR);
    Texture2D cubeRefl = load_texture("container2_specular.png", dir, aiTextureType_REFLECTION);

    Texture2D floorDiff = load_texture("plank_flooring_diff_1k.jpg", dir, aiTextureType_DIFFUSE);
    Texture2D floorSpec = load_texture("plank_flooring_rough_1k.jpg", dir, aiTextureType_SPECULAR);
//    Texture2D floorNorm = load_texture("plank_flooring_nor_gl_1k.jpg", dir, aiTextureType_NORMALS);
    Texture2D grassDiff = load_texture("grass.png", dir, aiTextureType_DIFFUSE, GL_CLAMP_TO_EDGE);
    Texture2D emptySpec = load_texture("pure_black.png", dir, aiTextureType_SPECULAR);
    Texture2D windowTexDiff = load_texture("window_transparent.png", dir, aiTextureType_DIFFUSE);

    GLuint skyBoxTex = load_cube_map(
            {"skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg"},
            dir
    );

    GLuint emptyEnvMap = load_cube_map(
            {"pure_black.png", "pure_black.png", "pure_black.png", "pure_black.png", "pure_black.png", "pure_black.png"},
            dir
    );

    GLuint emptyShadow = load_texture("pure_black.png", dir);

    // Models
    Model nanoSuitModel("resources/nanosuit/nanosuit.obj");
    ModelManager modelManager;
    {
        auto cube           = [=]() -> Model {return shapes::Cube(1, {cubeDiff, cubeSpec});};
        auto lightCube      = [=]() -> Model {return shapes::Cube(0.2f);};
        auto woodenFloor    = [=]() -> Model {return shapes::Rectangle(16, 16, {floorDiff, floorSpec});};
        auto rgbWindow      = [=]() -> Model {return shapes::Rectangle(1, 1, {windowTexDiff, emptySpec});};
        auto grass          = [=]() -> Model {return shapes::Rectangle(1, 1, {grassDiff, emptySpec});};

        modelManager.put(cube, "cube");
        modelManager.put(woodenFloor, "wooden_floor");
        modelManager.put(lightCube, "light_cube");
        modelManager.put(rgbWindow, "rgb_window");
        modelManager.put(grass, "grass");
    }

    // Sky box
    SkyBox* skyBox  = shapes::SkyBoxCube(skyBoxTex);


    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendEquation(GL_FUNC_ADD);   // default

    // Screen
    FrameBuffer frameBuffer(gameScrWidth, gameScrHeight, COLOR);
    frameBuffer.enableMSAA(COLOR | DEPTH | STENCIL, 4);
    Screen* screen  = shapes::ScreenRect({frameBuffer.getTexture()});

    int shadowRes = 4096;
    FrameBuffer directShadowMap(shadowRes, shadowRes, DEPTH, true);
    FrameBufferCube pointShadowMap(shadowRes, COLOR | DEPTH);
    Screen* depthScreen  = shapes::ScreenRect({directShadowMap.getTexture()});

    // Uniform buffer
    Buffer pvMatBuffer(GL_UNIFORM_BUFFER);
    pvMatBuffer.putData(SZ_MAT4F * 2, nullptr, GL_STATIC_DRAW);
    pvMatBuffer.bindBufferBase(0);


//    glm::vec3 pLight(1, 1, 1);
    PointLight pLight {
        glm::vec3(2.0f),
        glm::vec3(-1, 1, 1.5),
        1, 0.02, 25.0,
        pointShadowMap.getDepthCubeMap()
    };

    DirectionalLight dLight {
            glm::vec3(0.6f),
            glm::vec3(-1, -2, -1.5),
            glm::vec3(0.2f),
            directShadowMap.getDepthStencilTex()
    };


    Camera dLightCamera(dLight.injection * -3.0f , -glm::normalize(dLight.injection));
    glm::mat4 dLightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    glm::mat4 dLightView = dLightCamera.getView();

    double t0 = glfwGetTime();
    Camera camera(initPos);

    glGetError();
    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        // ...

        // rendering
        double t1 = glfwGetTime();
        gameSPF = static_cast<float>(t1 - t0);
        t0 = t1;
        glm::mat4 modelMtx(1.0f);
        // camera
        camera.move(cameraMov);
        camera.eulerAngle(playerPitch, playerYaw);
        const glm::mat4 view = camera.getView();
        const glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float) gameScrWidth / (float)gameScrHeight, 0.1f, 100.0f);
        const Model* model;

        // uniform buffer -- projection and view matrix
        pvMatBuffer.bind();
        // set view and projection
        pvMatBuffer.subData(0, SZ_MAT4F, glm::value_ptr(view));
        pvMatBuffer.subData(SZ_MAT4F, SZ_MAT4F, glm::value_ptr(proj));

        /*================ Shadow map ================*/
        glViewport(0, 0, shadowRes, shadowRes);
        directShadowMap.bind();

        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        depthShader->use();
        depthShader->uniformMatrix4fv(Shader::VIEW, dLightView);
        depthShader->uniformMatrix4fv(Shader::PROJECTION, dLightProj);
        depthShader->uniformMatrix4fv(Shader::MODEL, glm::translate(glm::mat4(1.0), glm::vec3(0, 0, -5.0)));
        glm::mat4 lightSpaceMtx = dLightProj * dLightView;

        // draw
        modelMtx = glm::mat4(1.0f);
        model = modelManager.getModel("cube");
        render(model, SHADOW, camera, modelMtx, pLight, dLight);

        modelMtx = glm::translate(modelMtx, glm::vec3(-1, 0, -2));
        render(model, SHADOW, camera, modelMtx, pLight, dLight);

        // two grass
        modelMtx = glm::translate(glm::mat4(1), glm::vec3(-1, 0, 0.5f));
        model = modelManager.getModel("grass");
        render(model, SHADOW, camera, modelMtx, pLight, dLight);

        modelMtx = glm::translate(modelMtx, glm::vec3(2, 0, 0));
        render(model, SHADOW, camera, modelMtx, pLight, dLight);

        directShadowMap.unbind();
        pointShadowMap.bind();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);


        modelMtx = glm::mat4(1.0f);
        model = modelManager.getModel("cube");
        renderCube(model, SHADOW, pLight.pos, &modelMtx, 1, pLight, dLight);

        modelMtx = glm::translate(modelMtx, glm::vec3(-1, 0, -2));
        renderCube(model, SHADOW, pLight.pos, &modelMtx, 1, pLight, dLight);

        // two grass
        modelMtx = glm::translate(glm::mat4(1), glm::vec3(-1, 0, 0.5f));
        model = modelManager.getModel("grass");
        renderCube(model, SHADOW, pLight.pos, &modelMtx, 1, pLight, dLight);

        modelMtx = glm::translate(modelMtx, glm::vec3(2, 0, 0));
        renderCube(model, SHADOW, pLight.pos, &modelMtx, 1, pLight, dLight);


        glViewport(0, 0, gameScrWidth, gameScrHeight);


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


        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);


        /*================ solid items ================*/

//        solidShader.uniformVec3("spotLight.color", pLight);
//        solidShader.uniformVec3("spotLight.pos", camera.getPos());
//        solidShader.uniformVec3("spotLight.direction", camera.getFocal());
//        solidShader.uniformFloat("spotLight.cutOff", glm::cos(glm::radians(15.0f)));


        // open stencil test
//        glStencilFunc(GL_ALWAYS, 1, 0xFF);
//        glStencilMask(0xFF);

//        cube->setEnvironmentMap(emptyEnvMap);
//        nanoSuitModel.Draw(solidShader);
//        cube->draw(*solidShader);
        solidShader->use();
        solidShader->uniformMatrix4fv("lightSpaceMtx", lightSpaceMtx);

        modelMtx = glm::mat4(1.0f);
        model = modelManager.getModel("cube");
        render(model, SOLID, camera, modelMtx, pLight, dLight);


        modelMtx = glm::translate(modelMtx, glm::vec3(-1, 0, -2));
        render(model, SOLID, camera, modelMtx, pLight, dLight);

        // close stencil test
//        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);  // whe
//        glStencilMask(0x00);    // Disable writing


        /*================ instanced infinite woodenFloor ================*/
        const int range = 8;
        const int amount = (2 * range + 1) * (2 * range + 1);
        std::vector<glm::mat4> floorMats;
        floorMats.reserve(amount);
        glm::vec3 playerPos = camera.getPos();
        Long chunkX = static_cast<int>(playerPos.x) >> 4;
        Long chunkZ = static_cast<int>(playerPos.z) >> 4;
        for (int i = -range; i <= range; ++i) {
            for (int j = -range; j <= range; ++j) {
                Long x = (chunkX + i) << 4;
                Long z = (chunkZ + j) << 4;
                glm::vec3 floorPos(x, -0.501, z);
                modelMtx = glm::translate(glm::mat4(1.0f), floorPos);
                modelMtx = glm::rotate(modelMtx, glm::radians(-90.0f), CoordAxis::XP);
                floorMats.push_back(modelMtx);
            }
        }
        solidShader->use();
        solidShader->uniformMatrix4fv("lightSpaceMtx", lightSpaceMtx);
        model = modelManager.getModel("wooden_floor");
        render(model, SOLID, camera, &floorMats[0], floorMats.size(), pLight, dLight);


        /*================ point light ================*/
        simpleShader->use();
        simpleShader->uniformVec3("color", glm::vec3(1, 1, 1));
        modelMtx = glm::mat4(1.0f);
        modelMtx = glm::translate(modelMtx, pLight.pos);
        model = modelManager.getModel("light_cube");
        render(model, PURE, camera, modelMtx, pLight, dLight);


        /*================ sky box ================*/
//        skyShader.use();
//        skyShader.uniformMatrix4fv(Shader::PROJECTION, proj);
//        skyShader.uniformMatrix4fv("view", glm::mat4(glm::mat3(view)));
//        skyBox->draw(skyShader);


        /*================ cutout items ================*/
        // two grass
        modelMtx = glm::translate(glm::mat4(1), glm::vec3(-1, 0, 0.5f));
        model = modelManager.getModel("grass");
        render(model, CUTOUT, camera, modelMtx, pLight, dLight);

        modelMtx = glm::translate(modelMtx, glm::vec3(2, 0, 0));
        render(model, CUTOUT, camera, modelMtx, pLight, dLight);



        /*================ draw outline (result from stencil test) ================*/
//        glDisable(GL_DEPTH_TEST);
//        simpleShader.use();
//        simpleShader.uniformVec3("viewPos", camera.getPos());
//        simpleShader.uniformMatrix4fv(Shader::PROJECTION, proj);
//        simpleShader.uniformMatrix4fv("view", view);
//
//        modelMtx = glm::scale(glm::mat4(1), glm::vec3(1.1));
//        simpleShader.uniformMatrix4fv(Shader::MODEL, modelMtx);
//        simpleShader.uniformVec3("color", glm::vec3(1, 0, 1));
//        cube->draw(simpleShader);
//        glStencilMask(0xFF);
//        glEnable(GL_DEPTH_TEST);


        /*================ Transparent: two rgbWindows ================*/
        transparentShader->use();
        setupLight(transparentShader, pLight, dLight);
        transparentShader->uniformVec3("viewPos", camera.getPos());

        glm::vec3 poses[3];
        poses[0] = glm::vec3(-2, 0, 2.5f);
        poses[1] = glm::vec3(1, 0, 1.5f);
        poses[2] = glm::vec3(-3, 0, 1.5f);
        auto cmp = [&] (const glm::vec3& v1, glm::vec3& v2) {
            return glm::distance(camera.getPos(), v1) > glm::distance(camera.getPos(), v2);
        };
        std::sort(poses, poses+3, cmp);
        model = modelManager.getModel("rgb_window");
        for (const auto& pos : poses) {
            glm::mat4 m(1);
            render(model, TRANSPARENT, camera, glm::translate(m, pos), pLight, dLight);
        }


        /*================ Post-Production ================*/
        frameBuffer.unbind();
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);
        screenShader->use();
        screen->draw(*screenShader);

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


