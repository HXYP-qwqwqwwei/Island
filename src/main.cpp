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
#include "util/render_util.h"
#include "util/world.h"

using GLObject = GLuint;
using GLLoc = GLint;

#define ISLAND_ENABLE_HDR
//#define ISLAND_ENABLE_DEFERRED_SHADING

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
    glGetError();

    // compile Shader programs
    compileShaders();

    // 在开始渲染前，需要告诉OpenGL窗口的坐标和大小
    // 因为OpenGL使用的任何坐标范围是[-1, +1]，通过指定大小后就可以将其映射到[0, W]和[0, H]
    // 例如(-0.5, 0.5)将会被映射到屏幕的(W/4, 3H/4)
    glViewport(0, 0, GameScrWidth, GameScrHeight);


    // Hide cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    const std::string dir = "resources/textures";
    textures::loadDefaultTextures(dir);
    Texture2D cubeDiff = load_texture("container2.png", dir, aiTextureType_DIFFUSE);
    Texture2D cubeSpec = load_texture("container2_specular.png", dir, aiTextureType_SPECULAR);
    Texture2D cubeRefl = load_texture("container2_specular.png", dir, aiTextureType_REFLECTION);
    Texture2D cubeNorm = load_texture("container2_normals.png", dir, aiTextureType_NORMALS);


    Texture2D toyBoxDiff = load_texture("toy_box_diffuse.png", dir, aiTextureType_DIFFUSE);
    Texture2D toyBoxSpec = {textures::WHITE_RGB, aiTextureType_SPECULAR};
    Texture2D toyBoxNorm = load_texture("toy_box_normal.png", dir, aiTextureType_NORMALS);
    Texture2D toyBoxPara = load_texture("toy_box_disp.png", dir, aiTextureType_DISPLACEMENT);

    Texture2D floorDiff = load_texture("plank_flooring_diff_1k.jpg", dir, aiTextureType_DIFFUSE);
    Texture2D floorSpec = load_texture("plank_flooring_rough_1k.jpg", dir, aiTextureType_SPECULAR);
    Texture2D floorNorm = load_texture("plank_flooring_nor_gl_1k.jpg", dir, aiTextureType_NORMALS);
//    Texture2D floorPara = load_texture("plank_flooring_disp_1k.jpg", dir, aiTextureType_DISPLACEMENT);
    Texture2D grassDiff = load_texture("grass.png", dir, aiTextureType_DIFFUSE, GL_CLAMP_TO_EDGE);
    Texture2D windowTexDiff = load_texture("window_transparent.png", dir, aiTextureType_DIFFUSE);

    GLuint skyBoxTex = load_cube_map(
            {"skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg"},
            dir
    );

    // Models
    Model nanoSuitModel("resources/nanosuit/nanosuit.obj");
    {
        auto cube           = [=]() -> Model {return shapes::Cube(1, {cubeDiff, cubeRefl, cubeSpec, cubeNorm});};
        auto lightCube      = [=]() -> Model {return shapes::Cube(0.5f);};
        auto woodenFloor    = [=]() -> Model {return shapes::Rectangle(16, 16, {floorDiff, floorSpec, floorNorm});};
        auto rgbWindow      = [=]() -> Model {return shapes::Rectangle(1, 1, {windowTexDiff});};
        auto grass          = [=]() -> Model {return shapes::Rectangle(1, 1, {grassDiff});};
        auto toyBox         = [=]() -> Model {
            return shapes::Cube(1, {toyBoxDiff, toyBoxNorm, toyBoxPara, toyBoxSpec});
        };

        MODEL_MANAGER.put(cube, "cube");
        MODEL_MANAGER.put(woodenFloor, "wooden_floor");
        MODEL_MANAGER.put(lightCube, "light_cube");
        MODEL_MANAGER.put(rgbWindow, "rgb_window");
        MODEL_MANAGER.put(grass, "grass");
        MODEL_MANAGER.put(toyBox, "toy_box");
    }

    // Sky box
    SkyBox* skyBox  = shapes::SkyBoxCube(skyBoxTex);

    // Screen
#ifdef ISLAND_ENABLE_DEFERRED_SHADING
    // G-Buffer
    FrameBuffer gBuffer(GameScrWidth, GameScrHeight);
    gBuffer.texture(RGB_FLOAT, 2)   // position & normals
            .texture(RGB_BYTE, 2)   // diffuse & specular
            .depthBuffer().stencilBuffer().useRenderBuffer()
            .build();

    Screen* screen = shapes::ScreenRect({
        gBuffer.getTexture(0),
        gBuffer.getTexture(1),
        gBuffer.getTexture(2),
        gBuffer.getTexture(3),
    });

#elif defined(ISLAND_ENABLE_HDR)
    FrameBuffer frameBuffer(GameScrWidth, GameScrHeight);
    frameBuffer.texture(RGB_FLOAT, 2).depthBuffer().stencilBuffer().useRenderBuffer().build();
    Screen* bright = shapes::ScreenRect({frameBuffer.getTexture(1)});
    // For Bloom
    FrameBuffer* pingPongBuffer[2];
    Screen* blurredBright[2];
    for (int i = 0; i < 2; ++i) {
        auto* buffer = new FrameBuffer(GameScrWidth, GameScrHeight);
        buffer->texture(RGB_FLOAT).build();
        pingPongBuffer[i] = buffer;
        blurredBright[1 - i] = shapes::ScreenRect({pingPongBuffer[i]->getTexture()});
    }

    Screen* screen = shapes::ScreenRect({frameBuffer.getTexture(), pingPongBuffer[0]->getTexture()});
#else
    FrameBuffer frameBuffer(GameScrWidth, GameScrHeight, RGB_BYTE);
    frameBuffer.enableMSAA(RGB_BYTE | DEPTH | STENCIL, 4);
    Screen* screen = shapes::ScreenRect({frameBuffer.getTexture()});
#endif


    double t0 = glfwGetTime();
    Camera camera(initPos);
    GLuint envMap = textures::EMPTY_ENV_MAP;

    InitWorld();
    SetDirectLight(glm::vec3(-1, -2, -1.5), glm::vec3(.0f), 4096);
    CreatePointLight(glm::vec3(-3, 1, -3), glm::vec3(30.0f), 4096);
    CreatePointLight(glm::vec3( 3, 1, -3), glm::vec3(30.0f, 0, 0), 4096);
    CreatePointLight(glm::vec3(-3, 1,  3), glm::vec3(0, 30.0f, 0), 4096);
    CreatePointLight(glm::vec3( 3, 1,  3), glm::vec3(0, 0, 30.0f), 4096);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        // input
        // ...
        int err = glGetError();
        if (err != 0) {
            std::cout << "ERROR::OpenGL code: " << err << '\n';
        }
        // rendering
        double t1 = glfwGetTime();
        gameSPF = static_cast<float>(t1 - t0);
        t0 = t1;

//        pLight.pos = glm::vec3(3*cos(t1) - 1, 1, 3*sin(t1) - 1);

        glm::mat4 modelMtx(1.0f);
        // camera
        camera.move(cameraMov);
        camera.eulerAngle(playerPitch, playerYaw);


        /*================ Decorate World Objects ================*/
        modelMtx = glm::mat4(1.0);
        ModelInfo boxes = MODEL_MANAGER.createInfo("cube");
        boxes.addInstance(modelMtx);
        boxes.addInstance(glm::translate(modelMtx, glm::vec3(-1, 0, -2)));
        PutModelInfo(SOLID, &boxes);

        ModelInfo toyBoxes = MODEL_MANAGER.createInfo("toy_box");
        toyBoxes.addInstance(glm::translate(modelMtx, glm::vec3(-2, 0, -4)));
        PutModelInfo(SOLID, &toyBoxes);

        const int range = 8;
        const int amount = (2 * range + 1) * (2 * range + 1);
        std::vector<glm::mat4> floorMats;
        floorMats.reserve(amount);
        glm::vec3 playerPos = camera.getPos();
        Long chunkX = static_cast<Long>(playerPos.x) >> 4;
        Long chunkZ = static_cast<Long>(playerPos.z) >> 4;
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
        ModelInfo floors = MODEL_MANAGER.createInfo("wooden_floor");
        floors.addInstance(floorMats);
        PutModelInfo(SOLID, &floors);


        ModelInfo grasses = MODEL_MANAGER.createInfo("grass");
        modelMtx = glm::mat4(1);
        grasses.addInstance({
                glm::translate(modelMtx, glm::vec3(-1, 0, 0.5f)),
                glm::translate(modelMtx, glm::vec3(1, 0, 0.5f))
        });
        PutModelInfo(CUTOUT, &grasses);

//        ModelInfo rgb_windows = MODEL_MANAGER.createInfo("rgb_window");
//        glm::vec3 poses[3];
//        poses[0] = glm::vec3(-2, 0, 2.5f);
//        poses[1] = glm::vec3(1, 0, 1.5f);
//        poses[2] = glm::vec3(-3, 0, 1.5f);
//        auto cmp = [&] (const glm::vec3& v1, glm::vec3& v2) {
//            return glm::distance(camera.getPos(), v1) > glm::distance(camera.getPos(), v2);
//        };
//        std::sort(poses, poses+3, cmp);
//
//        rgb_windows.addInstance({
//                glm::translate(modelMtx, poses[0]),
//                glm::translate(modelMtx, poses[1]),
//                glm::translate(modelMtx, poses[2])
//        });
//        PutModelInfo(TRANSPARENT, &rgb_windows);

#ifdef ISLAND_ENABLE_DEFERRED_SHADING
        RenderWorldGBuffer(camera, gBuffer);
        Flush();
#else
        RenderShadow();
//        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        RenderWorld(camera, frameBuffer);
        Flush();
#endif

        /*================ sky box ================*/
////        SkyShader.use();
////        SkyShader.uniformMatrix4fv(Shader::PROJECTION, proj);
////        SkyShader.uniformMatrix4fv("view", glm::mat4(glm::mat3(view)));
////        skyBox->draw(SkyShader);
        /*================ Post-Production ================*/
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);




#ifdef ISLAND_ENABLE_DEFERRED_SHADING
        DeferredShader->use();
        screen->draw(*DeferredShader);
#elif defined(ISLAND_ENABLE_HDR)
        GaussianBlurShader->use();
        int gaussianLevels = 10;
        bool horizontal = false;
        GaussianBlurShader->uniformBool(Shader::GAUSSIAN_HORIZONTAL, horizontal);
        pingPongBuffer[0]->bind();
        bright->draw(*GaussianBlurShader);
        int cycles = 2 * gaussianLevels - 1;
        for (int i = 0; i < cycles; ++i) {
            horizontal = !horizontal;
            pingPongBuffer[horizontal]->bind();
            GaussianBlurShader->uniformBool(Shader::GAUSSIAN_HORIZONTAL, horizontal);
            blurredBright[horizontal]->draw(*GaussianBlurShader);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        ScreenShaderHDR->use();
        screen->draw(*ScreenShaderHDR);
#else
        ScreenShader->use();
        screen->draw(*ScreenShader);
#endif

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


