//
// Created by HXYP on 2023/7/19.
//

#include "util/world.h"

static std::unordered_map<RenderType, std::vector<const ModelInfo*>> Models;

static std::vector<PointLight*> PointLights;
static std::vector<FrameBufferCube*> PointShadowBuffers;

static DirectionalLight DirectLight;
static FrameBuffer* DirectShadowBuffer = nullptr;

static Buffer* PVMatBuffer;
static const FrameBuffer* BoundFrame;
static const FrameBuffer* PingPongFrames[2];
static Screen* screen;
ModelManager MODEL_MANAGER;


void UnbindAllTextures();

void InitWorld() {
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendEquation(GL_FUNC_ADD);   // default

    for (auto & frame: PingPongFrames) {
        auto* buffer = new FrameBuffer(GameScrWidth, GameScrHeight);
        buffer->texture(RGB_FLOAT).build();
        frame = buffer;
    }

    PVMatBuffer = new Buffer(GL_UNIFORM_BUFFER);
    PVMatBuffer->putData(SZ_MAT4F * 2, nullptr, GL_STATIC_DRAW);
    PVMatBuffer->bindBufferBase(0);
    screen = shapes::ScreenRect();
}


void PutModelInfo(RenderType type, const ModelInfo* modelInfo) {
    Models[type].push_back(modelInfo);
}


void Flush() {
    Models.clear();
}

void BindFrameBuffer(const FrameBuffer* frame) {
    if (frame == nullptr) {
        if (BoundFrame != nullptr) {
            BoundFrame->unbind();
            BoundFrame = nullptr;
        }
        return;
    }
    GLsizei width = frame->width;
    GLsizei height = frame->height;
    glViewport(0, 0, width, height);
    frame->bind();
    BoundFrame = frame;
}

void ClearBuffer(int bits) {
    glClearColor(DirectLight.ambient.r, DirectLight.ambient.g, DirectLight.ambient.b, 1.0F);
    // 清除(上一帧的)颜色/深度测试/模板测试缓冲
    glClear(bits);
}

void ClearBuffer(int bits, float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    glClear(bits);
}


void RenderModelsInWorld(Camera &camera, RenderType type) {

    Light light(DirectLight, PointLights);

    const std::vector<const ModelInfo*>& models = Models[type];
    if (type == PURE) {
        ModelInfo lightCubes = MODEL_MANAGER.createInfo("light_cube");
        glm::mat4 E(1.0f);
        for (auto p: PointLights) {
            glm::mat4 trans = glm::translate(E, p->pos);
            renderPureColor(MODEL_MANAGER[lightCubes.id], &trans, 1, p->color);
        }

    } else {
        for (auto* info: models) {
            render(MODEL_MANAGER[info->id], type, camera, &info->transMatrices[0], info->transMatrices.size(), light);
        }
    }

    UnbindAllTextures();
}

void RenderWorldGBuffer(Camera& camera, RenderType type) {
    // Normal Models
    const std::vector<const ModelInfo*>& models = Models[type];

    for (auto* info: models) {
        renderGBuffer(MODEL_MANAGER[info->id], type, camera, &info->transMatrices[0], info->transMatrices.size());
    }
    UnbindAllTextures();
}


void SetupPVMatrix(Camera &camera, float zNear, float zFar) {
    GLsizei width;
    GLsizei height;

    if (BoundFrame == nullptr) {
        width = GameScrWidth;
        height = GameScrHeight;
    } else {
        width = BoundFrame->width;
        height = BoundFrame->height;
    }
    float aspect = (float)width / (float)height;

    const glm::mat4 view = camera.getView();
    const glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, zNear, zFar);

    // uniform buffer -- projection and view matrix
    // set view and projection
    PVMatBuffer->bind();
    PVMatBuffer->subData(0, SZ_MAT4F, glm::value_ptr(view));
    PVMatBuffer->subData(SZ_MAT4F, SZ_MAT4F, glm::value_ptr(proj));
    PVMatBuffer->unbind();
}

void EnableDepthTest(GLenum func) {
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(func);
}

void DisableDepthTest() {
    glDisable(GL_DEPTH_TEST);
}



void RenderWorld(Camera& camera, FrameBuffer& frame) {

    GLsizei width = frame.width;
    GLsizei height = frame.height;
    glViewport(0, 0, width, height);
    frame.bind();

    glClearColor(DirectLight.ambient.r, DirectLight.ambient.g, DirectLight.ambient.b, 1.0F);

    // Depth test
    glDepthFunc(GL_LESS);   // Default
    // Stencil test
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  // when a fragment passed ST, replace its value
    // Blend
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 清除(上一帧的)颜色/深度测试/模板测试缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);


    const glm::mat4 view = camera.getView();
    const glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

    // uniform buffer -- projection and view matrix
    PVMatBuffer->bind();
    // set view and projection
    PVMatBuffer->subData(0, SZ_MAT4F, glm::value_ptr(view));
    PVMatBuffer->subData(SZ_MAT4F, SZ_MAT4F, glm::value_ptr(proj));

    // Light Cubes
    ModelInfo lightCubes = MODEL_MANAGER.createInfo("light_cube");
    glm::mat4 E(1.0f);
    for (auto p: PointLights) {
        glm::mat4 trans = glm::translate(E, p->pos);
        renderPureColor(MODEL_MANAGER[lightCubes.id], &trans, 1, p->color);
    }

    // Normal Models
    const std::vector<const ModelInfo*>* models;
    static const std::vector<RenderType> types{SOLID, CUTOUT, TRANSPARENT};

    Light light(DirectLight, PointLights);
    for (RenderType type: types) {
        models = &Models[type];
        for (auto* info: *models) {
            render(MODEL_MANAGER[info->id], type, camera, &info->transMatrices[0], info->transMatrices.size(), light);
        }
    }

    PVMatBuffer->unbind();
    frame.unbind();
    UnbindAllTextures();
}


//void RenderWorldGBuffer(Camera& camera, FrameBuffer& gBuffer) {
//
//    GLsizei width = gBuffer.width;
//    GLsizei height = gBuffer.height;
//    glViewport(0, 0, width, height);
//    gBuffer.bind();
//
//    glClearColor(DirectLight.ambient.r, DirectLight.ambient.g, DirectLight.ambient.b, 1.0F);
//
//    // Depth test
//    glDepthFunc(GL_LESS);   // Default
//    // Stencil test
//    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  // when a fragment passed ST, replace its value
//    // Blend
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//    // 清除上一帧的颜色/深度测试/模板测试缓冲
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//    glEnable(GL_DEPTH_TEST);
//    glDepthFunc(GL_LEQUAL);
//
//
//    const glm::mat4 view = camera.getView();
//    const glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
//
//    // uniform buffer -- projection and view matrix
//    PVMatBuffer->bind();
//    // set view and projection
//    PVMatBuffer->subData(0, SZ_MAT4F, glm::value_ptr(view));
//    PVMatBuffer->subData(SZ_MAT4F, SZ_MAT4F, glm::value_ptr(proj));
//
//    // Normal Models
//    const std::vector<const ModelInfo*>* models;
//    static const std::vector<RenderType> types{SOLID, CUTOUT, TRANSPARENT};
//
//    Light light(DirectLight, PointLights);
//    for (RenderType type: types) {
//        models = &Models[type];
//        for (auto* info: *models) {
//            renderGBuffer(MODEL_MANAGER[info->id], &info->transMatrices[0], info->transMatrices.size());
//        }
//    }
//
//    PVMatBuffer->unbind();
//    gBuffer.unbind();
//    UnbindAllTextures();
//}



void RenderShadow() {
    auto* buffer = DirectShadowBuffer;
    if (buffer == nullptr) {
        return;
    }
    glViewport(0, 0, buffer->width, buffer->height);
    buffer->bind();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);

    const std::vector<const ModelInfo*>* models;
    static const std::vector<RenderType> types{SOLID, CUTOUT};


    /*================== Directional Light Shadow ==================*/
    for (RenderType type: types) {
        models = &Models[type];
        for (auto* info: *models) {
            renderShadow(MODEL_MANAGER[info->id], &info->transMatrices[0], info->transMatrices.size(), DirectLight);
        }
    }
    buffer->unbind();


    /*================== Point Light Shadow ==================*/
    size_t amount = PointLights.size();
    for (size_t i = 0; i < amount; ++i) {
        const auto* shadowBuf = PointShadowBuffers[i];
        const auto* light = PointLights[i];
        glViewport(0, 0, shadowBuf->length, shadowBuf->length);
        shadowBuf->bind();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (RenderType type: types) {
            models = &Models[type];
            for (auto* info: *models) {
                renderPointShadow(MODEL_MANAGER[info->id], &info->transMatrices[0], info->transMatrices.size(), *light);
            }
        }
    }
    UnbindAllTextures();
}

void UnbindAllTextures() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


uint CreatePointLight(glm::vec3 pos, glm::vec3 color, GLsizei shadowRes, float linear, float zNear, float zFar) {
    uint id = PointLights.size();
    auto* shadowBuffer = new FrameBufferCube(shadowRes, DEPTH);
    auto* p = new PointLight{color, pos, linear, zNear, zFar, shadowBuffer->getDepthCubeMap()};

    PointShadowBuffers.push_back(shadowBuffer);
    PointLights.push_back(p);
    return id;
}


void SetDirectLight(glm::vec3 injection, glm::vec3 color, GLsizei shadowRes, glm::vec3 ambient) {
    Camera dLightCamera(injection * -30.0f , -glm::normalize(injection));
    glm::mat4 dLightProj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.01f, 1000.0f);
    glm::mat4 dLightView = dLightCamera.getView();

    delete DirectShadowBuffer;

    DirectLight = {color, injection, ambient};
    auto* shadowBuffer = new FrameBuffer(shadowRes, shadowRes);
    shadowBuffer->depthBuffer().build();
    DirectLight.shadow = shadowBuffer->getDepthStencilTex();
    DirectLight.spaceMtx = dLightProj * dLightView;

    DirectShadowBuffer = shadowBuffer;
}

void ProcessGBuffer(const Camera& camera, const FrameBuffer& gBuffer) {
    if (BoundFrame == nullptr) {
        std::cerr << "ERROR::WORLD::Not Bound any Buffer\n";
        return;
    }
    screen->setTextures({
        gBuffer.getTexture(0),
        gBuffer.getTexture(1),
        gBuffer.getTexture(2),
        gBuffer.getTexture(3),
    });
    processGBuffer(screen, camera, Light{DirectLight, PointLights});
    BoundFrame->blitDepth(gBuffer, GL_DEPTH_BUFFER_BIT);
}


void RenderScreen(const FrameBuffer& frame, std::initializer_list<int> indices) {

    auto getTex = [&](int id) -> GLuint { return frame.getTexture(id); };

    std::vector<GLuint> textures(indices.size(), 0);
    std::transform(indices.begin(), indices.end(), textures.begin(), getTex);
    screen->setTextures(textures);
    ScreenShaderHDR->use();
    screen->draw(*ScreenShaderHDR);
}

void RenderScreen() {
    ScreenShaderHDR->use();
    screen->draw(*ScreenShaderHDR);
}


GLuint Blur(int id, int level) {
    if (BoundFrame == nullptr) {
        std::cerr << "WARN::WORLD::Cannot blit when bound default frameBuffer\n";
        return 0;
    }
    if (level == 0) return BoundFrame->getTexture(id);
    GLuint tex = BoundFrame->getTexture(id);


    GaussianBlurShader->use();
    bool horizontal = false;
    GaussianBlurShader->uniformBool(Shader::GAUSSIAN_HORIZONTAL, horizontal);

    PingPongFrames[0]->bind();
    screen->setTextures({tex});
    screen->draw(*GaussianBlurShader);

    int cycles = 2 * level - 1;
    for (int i = 0; i < cycles; ++i) {
        screen->setTextures({PingPongFrames[horizontal]->getTexture()});
        horizontal = !horizontal;
        PingPongFrames[horizontal]->bind();
        GaussianBlurShader->uniformBool(Shader::GAUSSIAN_HORIZONTAL, horizontal);
        screen->draw(*GaussianBlurShader);
    }
    BoundFrame->bind();
    return PingPongFrames[1]->getTexture();
}

void SetScreenTextures(const std::vector<GLuint> &textures) {
    screen->setTextures(textures);
}

void SetScreenTextures(const std::initializer_list<GLuint>& textures) {
    screen->setTextures(textures);
}


