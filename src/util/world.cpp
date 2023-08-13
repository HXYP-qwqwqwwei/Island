//
// Created by HXYP on 2023/7/19.
//

#include "util/world.h"

static std::unordered_map<RenderType, std::vector<const ModelInfo*>> Models;

static std::vector<PointLight*> PointLights;
static std::vector<PointLight*> PointLightsNoShadow;
static std::vector<FrameBufferCube*> PointShadowBuffers;

static DirectionalLight DirectLight;
static std::vector<FrameBuffer*> CSMBuffers;

static Buffer* PVMatBuffer;
static FrameBuffer* BoundFrame;

static FrameBuffer* BlurRGBBuffer;
static FrameBuffer* BlurREDBuffer;
static Screen* screen;
ModelManager MODEL_MANAGER;


void UnbindAllTextures();

void PutCSMFrustums(Camera &camera, std::vector<CascadedShadowMap> &shadows);

void InitWorld() {
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);

    BlurRGBBuffer = &((new FrameBuffer(GameScrWidth, GameScrHeight))->texture(GL_RGB16F));
    BlurRGBBuffer->build();

    BlurREDBuffer = &((new FrameBuffer(GameScrWidth, GameScrHeight))->texture(GL_R16F));
    BlurREDBuffer->build();


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

void BindFrameBuffer(FrameBuffer* frame) {
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
    ClearBuffer(bits, DirectLight.ambient.r, DirectLight.ambient.g, DirectLight.ambient.b);
}

void ClearBuffer(int bits, float r, float g, float b) {
    glClearColor(r, g, b, 1.0f);
    if (bits & GL_STENCIL_BUFFER_BIT) {
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
    }
    glClear(bits);
}


void RenderModelsInWorld(Camera &camera, RenderType type) {

    Light light(DirectLight, PointLights);

    const std::vector<const ModelInfo*>& models = Models[type];
    if (type == PURE) {
        ModelInfo lightCubes = MODEL_MANAGER.createInfo("light_cube");
        if (!PointLights.empty()) {
            renderLightModels(MODEL_MANAGER[lightCubes.id], &PointLights[0], PointLights.size());
        }
        if (!PointLightsNoShadow.empty()) {
            renderLightModels(MODEL_MANAGER[lightCubes.id], &PointLightsNoShadow[0], PointLightsNoShadow.size());
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


void SetupPVMatrix(Camera &camera) {
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
    const glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, camera.near(), camera.far());

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
        renderLightModels(MODEL_MANAGER[lightCubes.id], &p, 1);
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


void RenderShadow(Camera& camera) {
    const std::vector<const ModelInfo *> *models;
    static const std::vector<RenderType> types{SOLID, CUTOUT};

    /*================== Directional Light Shadow ==================*/
    PutCSMFrustums(camera, DirectLight.shadowMaps);

    for (GLsizei i = 0; i < CSMBuffers.size(); ++i) {
        auto buffer = CSMBuffers[i];
        glViewport(0, 0, buffer->width, buffer->height);
        buffer->bind();
        glEnable(GL_DEPTH_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);


        for (RenderType type: types) {
            models = &Models[type];
            for (auto *info: *models) {
                renderShadow(MODEL_MANAGER[info->id], &info->transMatrices[0], info->transMatrices.size(), DirectLight, i);
            }
        }
        buffer->unbind();
    }


    /*================== Point Light Shadow ==================*/
    size_t amount = PointLights.size();
    for (size_t i = 0; i < amount; ++i) {
        const auto* shadowBuf = PointShadowBuffers[i];
        if (shadowBuf == nullptr) continue;
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

void PutCSMFrustums(Camera &camera, std::vector<CascadedShadowMap> &csmShadows) {
    float aspect = (float)GameScrWidth / (float)GameScrHeight;
    size_t nSeg = csmShadows.size();

    struct ViewFrustum {
        GLfloat near;
        GLfloat far;
        glm::vec3 vertices[8]{};
    };

    std::vector<ViewFrustum> viewFrustums;
    viewFrustums.reserve(nSeg);

    // 视锥体分块
    GLfloat near = camera.near();
    GLfloat far = camera.far();
    GLfloat fragFN = far / near;
    GLfloat lambda = 0.6;
    GLfloat ni = near;
    GLfloat fi;
    for (GLsizei i = 1; i <= nSeg; ++i) {
        GLfloat si = GLfloat(i)/GLfloat(nSeg);
        fi = lambda * near * pow(fragFN, si) + (1-lambda) * (near + (far-near) * si);
        viewFrustums.emplace_back(ni, fi * 1.05f);
        ni = fi;
    }

    // 计算各段视锥体的顶点坐标（世界空间）
    const glm::mat4 view = camera.getView();
    glm::mat3 view3x3T = glm::transpose(glm::mat3(view));
    const glm::vec3& right = view3x3T[0];
    const glm::vec3& up    = view3x3T[1];
    const glm::vec3& focal = view3x3T[2];
    GLfloat fov = glm::radians(45.0f);
    for (auto& frustum: viewFrustums) {
        ni = frustum.near;
        fi = frustum.far;
        GLfloat hNear = ni * glm::tan(fov/2);
        GLfloat wNear = hNear * aspect;
        GLfloat hFar  = fi * glm::tan(fov/2);
        GLfloat wFar  = hFar  * aspect;
        glm::vec3 vNear = camera.getPos() - focal * ni;
        glm::vec3 vFar  = camera.getPos() - focal * fi;

        frustum.vertices[0] = vNear + right*wNear + up*hNear;   // ru
        frustum.vertices[1] = vNear - right*wNear + up*hNear;   // lu
        frustum.vertices[2] = vNear - right*wNear - up*hNear;   // ld
        frustum.vertices[3] = vNear + right*wNear - up*hNear;   // rd

        frustum.vertices[4] = vFar + right*wFar + up*hFar;      // ru
        frustum.vertices[5] = vFar - right*wFar + up*hFar;      // lu
        frustum.vertices[6] = vFar - right*wFar - up*hFar;      // ld
        frustum.vertices[7] = vFar + right*wFar - up*hFar;      // rd
    }

    Camera LiCam(DirectLight.injection * -30.0f , -glm::normalize(DirectLight.injection));
    const glm::mat4 LiView = LiCam.getView();
    const glm::mat4 view_inv = glm::inverse(view);

    for (size_t i = 0; i < nSeg; ++i) {
        const auto& frustum = viewFrustums[i];
        auto& shadow = csmShadows[i];

        // 为每个分段计算光空间对应的变换矩阵
        GLfloat zMax = -FLOAT_INF;  // LiSpace
        GLfloat zMin =  FLOAT_INF;  // LiSpace
        for (const auto& v: frustum.vertices) {
            glm::vec4 v_LiView = LiView * glm::vec4(v, 1.0);
            zMax = MAX(zMax, v_LiView.z);
            zMin = MIN(zMin, v_LiView.z);
        }
        zMax += 10.0f;
        glm::mat4 LiProj = glm::ortho(-1.0f, +1.0f, -1.0f, +1.0f, -zMax, -zMin);
        const glm::mat4 LiPV = LiProj * LiView;

        // 为了使视锥体恰好被光源的PV空间包围，对P矩阵的上下左右平面进行放缩
        // 首先使用NDC的左右/上下平面作正射投影，计算xy坐标最大值
        glm::vec2 xyMax(-FLOAT_INF, -FLOAT_INF);  // LiSpace
        glm::vec2 xyMin( FLOAT_INF,  FLOAT_INF);  // LiSpace
        for (const auto& v: frustum.vertices) {
            glm::vec4 v_LProjView = LiPV * glm::vec4(v, 1.0);
            v_LProjView /= v_LProjView.w;
            xyMax.x = MAX(xyMax.x, v_LProjView.x);
            xyMax.y = MAX(xyMax.y, v_LProjView.y);
            xyMin.x = MIN(xyMin.x, v_LProjView.x);
            xyMin.y = MIN(xyMin.y, v_LProjView.y);
        }

        // 利用得到的左右/上下平面，计算放缩矩阵
        // [ Sx  0   0   Ox ] [ 1    0     0            0      ]     [ 2/(r-l)    0        0     -(r+l)/(r-l) ]
        // |     Sy      Oy | |      1     0            0      |     |         2/(t-b)           -(t+b)/(t-b) |
        // |         1      | |         -2/(f-n)  -(f+n)/(f-n) | ==> |                 -2/(f-n)  -(f+n)/(f-n) |
        // [             1  ] [                         1      ]     [                                 1      ]
        // 根据上式计算Sx, Sy, Ox, Oy
        // Sx = 2/(r-l),    Sy = 2/(t-b),   Ox = -(r+l)/(r-l),  Oy = -(t+b)/(t-b)
        glm::vec2 scale = 2.0f / (xyMax - xyMin);
        glm::vec2 offset = -0.5f * (xyMax + xyMin) * scale;

        glm::mat4 crop {
                scale.x,  0,        0,        0,
                0,        scale.y,  0,        0,
                0,        0,        1.0f,     0,
                offset.x, offset.y, 0,        1.0f
        };

        glm::mat4 NDC_to_01 {
                .5f,  0 ,  0 ,  0 , // [ .5   0   0  .5 ]
                0 , .5f,  0 ,  0 ,  // |     .5   0  .5 |
                0 ,  0 , .5f,  0 ,  // |         .5  .5 |
                .5f, .5f, .5f,  1   // [              1 ]
        };
        shadow.LiSpacePV = crop * LiPV;
        shadow.V2LiSpacePV = NDC_to_01 * shadow.LiSpacePV * view_inv;

        // 最后设置视锥体的远平面在投影空间的深度（范围为[zNear, zFar]，和G缓冲中的深度一致），以供片段着色器比较
        shadow.farDepth = frustum.far;
    }
}

void UnbindAllTextures() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


uint CreatePointLight(glm::vec3 pos, glm::vec3 color, GLsizei shadowRes, glm::vec3 attenu, glm::vec2 zNearFar) {
    uint id = PointLights.size();
    auto* shadowBuffer = new FrameBufferCube(shadowRes, GL_NONE, true);
    auto* p = new PointLight{color, pos, attenu, zNearFar, shadowBuffer->getDepthCubeMap()};

    PointShadowBuffers.push_back(shadowBuffer);
    PointLights.push_back(p);
    return id;
}


uint CreatePointLightNoShadow(glm::vec3 pos, glm::vec3 color, glm::vec3 attenu) {
    uint id = PointLights.size();
    auto* p = new PointLight{color, pos, attenu, {0.1f, 25.0f}, EMPTY_POINT_LIGHT.shadow};

    PointLightsNoShadow.push_back(p);
    return id;
}



void SetDirectLight(glm::vec3 injection, glm::vec3 color, GLsizei shadowRes, GLsizei csmLevels, glm::vec3 ambient) {
    for (auto buffer: CSMBuffers) {
        delete buffer;
    }
    CSMBuffers.clear();
    CSMBuffers.reserve(csmLevels);

    DirectLight = {color, injection, ambient};
    DirectLight.shadowMaps.reserve(csmLevels);

    for (GLsizei i = 0; i < csmLevels; ++i) {
        auto* shadowBuffer = new FrameBuffer(shadowRes, shadowRes);
        shadowBuffer->depthBuffer().build();
        DirectLight.shadowMaps.emplace_back(shadowBuffer->getDepthStencilTex());
        CSMBuffers.push_back(shadowBuffer);
    }
}

void ProcessGBuffer(const FrameBuffer& gBuffer, const Texture2D& ssao) {
    static auto* ball = shapes::BallMesh(1, 20, 10);
    static auto mesh = ball->build();
    if (BoundFrame == nullptr) {
        std::cerr << "ERROR::WORLD::Not Bound any Buffer\n";
        return;
    }
    screen->setTextures({
        gBuffer.getTexture(0),
        gBuffer.getTexture(1),
        gBuffer.getTexture(2),
        gBuffer.getTexture(3),
        ssao
    });
    mesh.setTextures({
         {gBuffer.getTexture(0)},
         {gBuffer.getTexture(1)},
         {gBuffer.getTexture(2)},
         {gBuffer.getTexture(3)},
    });


    lightGBuffer(screen, DirectLight);
    BoundFrame->blitDepth(gBuffer, GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    if (!PointLights.empty()) {
        glEnable(GL_STENCIL_TEST);
        glDepthMask(GL_FALSE);      // 禁止写入深度缓冲

        for (const auto* pLight: PointLights) {
            glClear(GL_STENCIL_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);
            glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);   // 剔除背面进行着色，注意即使没有通过深度测试，也要写入模板缓冲
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilMask(0xFF);

            lightGBuffer(&mesh, *pLight);

            glCullFace(GL_FRONT);
            glDisable(GL_DEPTH_TEST);
            glStencilMask(0x00);
            glStencilFunc(GL_NOTEQUAL, 1, 0xFF);    // 如果摄像机在球体内，上面的着色不会生效，但是模板缓冲值均为0，因此进行补着色

            lightGBuffer(&mesh, *pLight);

            glStencilMask(0xFF);            // 清除模板缓冲值
        }
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
    }

//    glCullFace(GL_FRONT);
//    glDisable(GL_DEPTH_TEST);
//    for (const auto* pLight: PointLights) {
//        lightGBuffer(&mesh, camera, *pLight);
//    }

    if (!PointLightsNoShadow.empty()) {
        glCullFace(GL_FRONT);
        glDisable(GL_DEPTH_TEST);
        lightGBufferNoShadow(&mesh, &PointLightsNoShadow[0], PointLightsNoShadow.size());
    }

    glDisable(GL_BLEND);

    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}


void RenderSSAO(const FrameBuffer& gBuffer, const glm::vec3* samples, size_t n, const Texture2D& texNoise, GLfloat radius, GLfloat power) {
    screen->setTextures({
        gBuffer.getTexture(0),
        gBuffer.getTexture(1),
        texNoise
    });

    const Shader* shader = SSAOShader;
    shader->use();
    for (size_t i = 0; i < n; ++i) {
        shader->uniformVec3(Shader::SSAO_SAMPLES + std::to_string(i) + ']', samples[i]);
    }
    shader->uniformFloat(Shader::SSAO_KERNEL_RADIUS, radius);
    shader->uniformFloat(Shader::SSAO_POWER, power);
    shader->uniformVec2(Shader::SCREEN_SIZE, glm::vec2(GameScrWidth, GameScrHeight));

    screen->draw(*shader);
    UnbindAllTextures();
}



void RenderFrame(const FrameBuffer& frame, std::initializer_list<int> indices) {

    auto getTex = [&](int id) -> Texture2D { return frame.getTexture(id); };

    std::vector<Texture2D> textures(indices.size());
    std::transform(indices.begin(), indices.end(), textures.begin(), getTex);
    screen->setTextures(textures);
    ScreenShaderHDR->use();
    screen->draw(*ScreenShaderHDR);
}

void RenderScreen() {
    ScreenShaderHDR->use();
    screen->draw(*ScreenShaderHDR);
}


void Blur(GLsizei index, GLsizei blurLevel) {
    if (BoundFrame == nullptr) {
        std::cerr << "WARN::WORLD::Cannot blit when bound default frameBuffer\n";
        return;
    }
    if (blurLevel <= 0) return;

    Texture2D tex = BoundFrame->getTexture(index);
    FrameBuffer* blurFrame;
    switch (tex.internalFormat) {
        case GL_R16F:
            blurFrame = BlurREDBuffer;
            break;
        case GL_RGB16F:
            blurFrame = BlurRGBBuffer;
            break;
        default:
            std::cerr << "ERROR::WORLD::Blurring not supported for textures with format " << tex.internalFormat << '\n';
            return;
    }


    GaussianBlurShader->use();
    bool horizontal = false;

    GLsizei cycles = 2 * blurLevel;
    for (GLsizei i = 0; i < cycles; ++i) {
        screen->setTextures({BoundFrame->getTexture(index)});
        GaussianBlurShader->uniformBool(Shader::GAUSSIAN_HORIZONTAL, horizontal);
        blurFrame->bind();
        screen->draw(*GaussianBlurShader);

        swapTexture(BoundFrame, blurFrame, index, 0);
        horizontal = !horizontal;
    }
}


