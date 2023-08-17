#include <iostream>
#include <random>
#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "util/shaders.h"
#include "util/game_util.h"
#include "util/shapes.h"
#include "util/Model.h"
#include "util/texture_util.h"
#include "util/buffer_util.h"
#include "util/render_util.h"
#include "util/world.h"

using GLObject = GLuint;
using GLLoc = GLint;

#define ISLAND_PBR
#define ISLAND_ENABLE_HDR
#define ISLAND_ENABLE_DEFERRED_SHADING
#undef ISLAND_ENABLE_DEFERRED_SHADING

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

    /**================ Load Models and Textures ================**/
    const std::string dir = "resources/textures";
    textures::loadDefaultTextures(dir);
    Texture2DWithType cubeDiff = load_texture("container2.png", dir, aiTextureType_DIFFUSE);
    Texture2DWithType cubeSpec = load_texture("container2_specular.png", dir, aiTextureType_SPECULAR);
    Texture2DWithType cubeRefl = load_texture("container2_specular.png", dir, aiTextureType_REFLECTION);
    Texture2DWithType cubeNorm = load_texture("container2_normals.png", dir, aiTextureType_NORMALS);


    Texture2DWithType toyBoxDiff = load_texture("toy_box_diffuse.png", dir, aiTextureType_DIFFUSE);
    Texture2DWithType toyBoxSpec = {textures::WHITE_RGB, aiTextureType_SPECULAR};
    Texture2DWithType toyBoxNorm = load_texture("toy_box_normal.png", dir, aiTextureType_NORMALS);
    Texture2DWithType toyBoxPara = load_texture("toy_box_disp.png", dir, aiTextureType_DISPLACEMENT);

    Texture2DWithType floorDiff = load_texture("plank_flooring_diff_1k.jpg", dir, aiTextureType_DIFFUSE);
    Texture2DWithType floorSpec = load_texture("plank_flooring_rough_1k.jpg", dir, aiTextureType_SPECULAR);
    Texture2DWithType floorNorm = load_texture("plank_flooring_nor_gl_1k.jpg", dir, aiTextureType_NORMALS);
//    Texture2DWithType floorPara = load_texture("plank_flooring_disp_1k.jpg", dir, aiTextureType_DISPLACEMENT);
    Texture2DWithType grassDiff = load_texture("grass.png", dir, aiTextureType_DIFFUSE, GL_CLAMP_TO_EDGE);
    Texture2DWithType windowTexDiff = load_texture("window_transparent.png", dir, aiTextureType_DIFFUSE);

    Texture2DWithType rustyIronDiff = load_texture("rustediron2_basecolor.png", dir, aiTextureType_DIFFUSE);
    Texture2DWithType rustyIronMetal = load_texture("rustediron2_metallic.png", dir, aiTextureType_METALNESS);
    Texture2DWithType rustyIronNorm = load_texture("rustediron2_normal.png", dir, aiTextureType_NORMALS);
    Texture2DWithType rustyIronRough = load_texture("rustediron2_roughness.png", dir, aiTextureType_DIFFUSE_ROUGHNESS);

    TextureCube skyBoxTex = load_cube_map(
            {"skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg"},
            dir
    );

    Texture2D loftEnv = load_texture_HDR("newport_loft.hdr", dir);

    // Models
    Model nanoSuitModel("resources/nanosuit/nanosuit.obj");
    {
        auto cube           = [=]() -> Model {return shapes::Cube(1, {cubeDiff, cubeRefl, cubeSpec, cubeNorm});};
        auto lightCube      = [=]() -> Model {return shapes::Ball(0.1f, 20, 10);};
        auto woodenFloor    = [=]() -> Model {return shapes::Rectangle(16, 16, {floorDiff, floorSpec, floorNorm});};
        auto rgbWindow      = [=]() -> Model {return shapes::Rectangle(1, 1, {windowTexDiff});};
        auto grass          = [=]() -> Model {return shapes::Rectangle(1, 1, {grassDiff});};
        auto toyBox         = [=]() -> Model {
            return shapes::Cube(1, {toyBoxDiff, toyBoxNorm, toyBoxPara, toyBoxSpec});
        };

        auto ball           = [=]() -> Model {
            return shapes::Ball(0.5f, 600, 300, {
                    rustyIronDiff,
                    rustyIronMetal,
                    rustyIronNorm,
                    rustyIronRough
            });
        };

        MODEL_MANAGER.put(cube, "cube");
        MODEL_MANAGER.put(woodenFloor, "wooden_floor");
        MODEL_MANAGER.put(lightCube, "light_cube");
        MODEL_MANAGER.put(rgbWindow, "rgb_window");
        MODEL_MANAGER.put(grass, "grass");
        MODEL_MANAGER.put(toyBox, "toy_box");
        MODEL_MANAGER.put(ball, "ball");
    }

    // Sky box
    SkyBox* skyBox  = shapes::SkyBoxCube(skyBoxTex);
    TextureCube& envMap = textures::EMPTY_ENV_MAP;

    /**================ Tools ================**/
    [[maybe_unused]] std::mt19937 randGen(std::random_device{}());
    std::uniform_real_distribution<float> u01(0, 1.0);


    /**================ Init FrameBuffers ================**/
#ifdef ISLAND_ENABLE_DEFERRED_SHADING
    // G-Buffer
    FrameBuffer gBuffer(GameScrWidth, GameScrHeight);
    gBuffer.texture(GL_RGBA16F)     // position & depth
            .texture(GL_RGB16F)      // specular
            .texture(GL_RGB, 2)   // diffuse & specular
            .withDepth().useRenderBuffer()
            .build();

    FrameBuffer frameBuffer(GameScrWidth, GameScrHeight);
    frameBuffer.texture(GL_RGB16F, 2).withDepth().withStencil().build();

    // ssao
    FrameBuffer ssaoFrame(GameScrWidth, GameScrHeight);
    ssaoFrame.texture(GL_R16F, 1, GL_CLAMP_TO_EDGE, GL_NEAREST).build();

    GLsizei nSamples = 64;
    GLsizei noiseSize = 4;
    std::vector<glm::vec3> ssaoSamples;
    ssaoSamples.reserve(nSamples);
    for (GLsizei i = 0; i < nSamples; ++i) {
        glm::vec3 sample {
                u01(randGen) * 2.0f - 1.0f,     // U(-1, 1)
                u01(randGen) * 2.0f - 1.0f,     // U(-1, 1)
                u01(randGen)                    // U(0, 1)
        };
        sample = glm::normalize(sample) * u01(randGen); // length -> U(0, 1);
        GLfloat scale = GLfloat(i) / GLfloat(nSamples);
        scale = std::lerp(0.1f, 1.0f, scale * scale);
        ssaoSamples.push_back(scale * sample);
    }

    GLsizei noiPixels = noiseSize * noiseSize;
    std::vector<glm::vec3> ssaoNoises;
    ssaoNoises.reserve(noiPixels);
    for (GLsizei i = 0; i < noiPixels; ++i) {
        ssaoNoises.emplace_back(
                u01(randGen) * 2.0f - 1.0f,
                u01(randGen) * 2.0f - 1.0f,
                0.0f
        );
    }

    Texture2D ssaoNoiseTex = createTexture2D(GL_RGB, GL_RGB16F, GL_FLOAT, noiseSize, noiseSize, &ssaoNoises[0], GL_REPEAT, GL_NEAREST, false);

#elif defined(ISLAND_ENABLE_HDR)
    FrameBuffer frameBuffer(GameScrWidth, GameScrHeight);
    frameBuffer.texture(GL_RGB16F, 2).withDepth().withStencil().useRenderBuffer().build();
#else
    FrameBuffer frameBuffer(GameScrWidth, GameScrHeight);
    frameBuffer.texture(GL_RGB).depthBuffer().stencilBuffer().useRenderBuffer().build();
#endif


    double t0 = glfwGetTime();
    double T0 = t0;
    int nFrames = 0;
    Camera camera(initPos);

    /**================ World and Lights ================**/
    InitWorld();
    SetDirectLight(glm::vec3(-1, -2, -1.5), glm::vec3(1.0f), 1024, 4, glm::vec3(.01));
    CreatePointLight(glm::vec3(-3, 1, -3), glm::vec3(10.0f), 512);
    CreatePointLight(glm::vec3( 3, 1, -3), glm::vec3(30.0f, 0, 0), 512);
    CreatePointLight(glm::vec3(-3, 1,  3), glm::vec3(0, 30.0f, 0), 512);
    CreatePointLight(glm::vec3( 3, 1,  3), glm::vec3(0, 0, 30.0f), 512);

#ifdef ISLAND_ENABLE_DEFERRED_SHADING
//    CreatePointLightNoShadow(glm::vec3(-3, 1, -3), glm::vec3(10.0f));
//    CreatePointLightNoShadow(glm::vec3( 3, 1, -3), glm::vec3(30.0f, 0, 0));
//    CreatePointLightNoShadow(glm::vec3(-3, 1,  3), glm::vec3(0, 30.0f, 0));
//    CreatePointLightNoShadow(glm::vec3( 3, 1,  3), glm::vec3(0, 0, 30.0f));
//
//
//    for (int i = -10; i <= 10; ++i) {
//        for (int j = -10; j <= 10; ++j) {
//            float r = u01(randGen) * 20;
//            float g = u01(randGen) * 20;
//            float b = u01(randGen) * 20;
//            CreatePointLightNoShadow(glm::vec3(i*3, 1, j*3), glm::vec3(r, g, b));
//        }
//    }
#endif


    /**================ Render Loop ================**/
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
        double T1 = t1;
        nFrames += 1;
        if ((Long)T1 != (Long)T0) {
            std::cout << "FPS: " << (int)(nFrames / (T1 - T0)) << '\n';
            nFrames = 0;
            T0 = T1;
        }

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

        ModelInfo ball = MODEL_MANAGER.createInfo("ball");
        ball.addInstance(glm::translate(modelMtx, glm::vec3(0, 2, 0)));
        PutModelInfo(PBR_SOLID, &ball);


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

/*
        ModelInfo rgb_windows = MODEL_MANAGER.createInfo("rgb_window");
        glm::vec3 poses[3];
        poses[0] = glm::vec3(-2, 0, 2.5f);
        poses[1] = glm::vec3(1, 0, 1.5f);
        poses[2] = glm::vec3(-3, 0, 1.5f);
        auto cmp = [&] (const glm::vec3& v1, glm::vec3& v2) {
            return glm::distance(camera.getPos(), v1) > glm::distance(camera.getPos(), v2);
        };
        std::sort(poses, poses+3, cmp);

        rgb_windows.addInstance({
                glm::translate(modelMtx, poses[0]),
                glm::translate(modelMtx, poses[1]),
                glm::translate(modelMtx, poses[2])
        });
        PutModelInfo(TRANSPARENT, &rgb_windows);
*/

        /*================ Render World ================*/
        SetupPVMatrix(camera);
        RenderShadow(camera);
#ifdef ISLAND_ENABLE_DEFERRED_SHADING
        BindFrameBuffer(&gBuffer);
        EnableDepthTest();
        ClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        RenderWorldGBuffer(camera, SOLID);
        RenderWorldGBuffer(camera, CUTOUT);
#else
        BindFrameBuffer(&frameBuffer);
        ClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        EnableDepthTest();
        RenderModelsInWorld(camera, SOLID);
        RenderModelsInWorld(camera, PBR_SOLID);
        RenderModelsInWorld(camera, CUTOUT);
        RenderModelsInWorld(camera, PURE);
#endif

        /*================ sky box ================*/
/*
        SkyShader.use();
        SkyShader.uniformMatrix4fv(Shader::PROJECTION, proj);
        SkyShader.uniformMatrix4fv("view", glm::mat4(glm::mat3(view)));
        skyBox->draw(SkyShader);
*/

#ifdef ISLAND_ENABLE_DEFERRED_SHADING
        BindFrameBuffer(&ssaoFrame);
        RenderSSAO(gBuffer, &ssaoSamples[0], ssaoSamples.size(), ssaoNoiseTex);
        Blur(0, 1);

        BindFrameBuffer(&frameBuffer);
        ClearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        ProcessGBuffer(gBuffer, ssaoFrame.getTexture());
        RenderModelsInWorld(camera, PURE);
#endif

        /*================ Post-Production ================*/
#ifdef ISLAND_ENABLE_HDR
        // Bloom
        Blur(1, 10);

        BindFrameBuffer(nullptr);
        DisableDepthTest();
        ClearBuffer(GL_COLOR_BUFFER_BIT, 1.0, 1.0, 1.0);
        RenderFrame(frameBuffer, {0, 1});
#else
        BindFrameBuffer(nullptr);
        ClearBuffer(GL_COLOR_BUFFER_BIT, 1.0, 1.0, 1.0);
        RenderFrame(frameBuffer, {0});
#endif
        Flush();
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


