//
// Created by HXYP on 2023/3/7.
//

#ifndef ISLAND_SHADERS_H
#define ISLAND_SHADERS_H

#include <string>
#include <iostream>
#include <fstream>
#include "defs.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "assimp/material.h"

#define SZ_MAT4F sizeof(glm::mat4)
#define SZ_MAT3F sizeof(glm::mat3)
#define SZ_VEC2F sizeof(glm::vec2)
#define SZ_VEC3F sizeof(glm::vec3)
#define SZ_VEC4F sizeof(glm::vec4)

using GLObject = GLuint;
using GLLoc = GLint;

enum RenderType {
    SOLID,
    CUTOUT,
    TRANSPARENT,
    SCREEN,
    SHADOW,
    PURE,
};


class Shader {
private:
    const GLObject shaderProgram;

    static bool compileShader(const std::string& source, GLObject& shaderObject, GLenum type) ;
    void attachShader(GLObject shader) const;

public:
    explicit Shader();
    void loadShader(const std::string& filename, GLenum type);
    void use() const;
    void link() const;
    void uniformMatrix4fv(const std::string& name, glm::mat4 matrix) const;
    void uniformVec3(const std::string& name, const glm::vec3& vec) const;
    void uniformVec2(const std::string& name, const glm::vec2& vec) const;
    void uniformFloat(const std::string& name, float fv) const;
    void uniformInt(const std::string& name, int iv) const;
    void uniformUInt(const std::string& name, GLuint uiv) const;
    void uniformBool(const std::string& name, bool bv) const;
    void setDefaultTexture(aiTextureType type, GLuint tex, int idx) const;
    void setEnvironmentMap(GLuint envMap) const;
    void setEnvironmentDiffuse(GLuint envDiff) const;
    void setEnvironmentPrefiltered(GLuint envPrefiltered) const;
    void setBRDFLookUpTex(GLuint brdfLUT) const;

    /*===== Uniform names =====*/
//    static constexpr const char MODEL[]                 = "model";
    static constexpr const char PVMatrix[] = "ProjViewMatrix";

    struct PointLight {
        static constexpr const char POS[] = "pointLight.pos";
        static constexpr const char COLOR[] = "pointLight.color";
        static constexpr const char ATTENU[] = "pointLight.attenu";
        static constexpr const char Z_NEAR_FAR[] = "pointLight.zNearFar";

    };

    struct DirectLight {
        static constexpr const char AMBIENT[] = "directLight.ambient";
        static constexpr const char COLOR[] = "directLight.color";
        static constexpr const char INJECTION[] = "directLight.injection";
        static constexpr const char CSM_LEVELS[] = "directLight.csmLevels";

        static constexpr const char CSM_MAPS[4][23] = {
                "directLight.csmMaps[0]",
                "directLight.csmMaps[1]",
                "directLight.csmMaps[2]",
                "directLight.csmMaps[3]",
        };

        static constexpr const char LiSPACE_MATRICES[4][31] = {
                "directLight.LiSpaceMatrices[0]",
                "directLight.LiSpaceMatrices[1]",
                "directLight.LiSpaceMatrices[2]",
                "directLight.LiSpaceMatrices[3]",
        };

        static constexpr const char FAR_DEPTHS[4][25] = {
                "directLight.farDepths[0]",
                "directLight.farDepths[1]",
                "directLight.farDepths[2]",
                "directLight.farDepths[3]",
        };


    };

    static constexpr const char VIEW_POS[]              = "viewPos";
    static constexpr const char COLOR[]                 = "color";
    static constexpr const char GAUSSIAN_HORIZONTAL[]   = "horizontal";
    static constexpr const char SHININESS[]             = "texes.shininess";
    static constexpr const char TEXTURES[]              = "texes.";
    static constexpr const char ENVIRONMENT_MAP[]       = "environment";
    static constexpr const char SSAO_SAMPLES[]          = "samples[";
    static constexpr const char SSAO_KERNEL_RADIUS[]    = "radius";
    static constexpr const char SSAO_POWER[]            = "power";
    static constexpr const char SCREEN_SIZE[]           = "screenSize";
    static constexpr const char N_SEGMENTS[]            = "nSegments";
    static constexpr const char ROUGHNESS[]             = "roughness";
    static constexpr const char N_SAMPLES[]             = "nSamples";

    static constexpr const char ENVIRONMENT_IRR[]       = "envIrradiance";
    static constexpr const char ENVIRONMENT_PF[]        = "envPrefiltered";
    static constexpr const char BRDF_LUT[]              = "brdfLookUpTex";

//    static constexpr const char PROJECTION[] = "proj";
    static constexpr const char CUBE_SPACE_MATRICES[6][21] = {
            "cubeSpaceMatrices[0]",
            "cubeSpaceMatrices[1]",
            "cubeSpaceMatrices[2]",
            "cubeSpaceMatrices[3]",
            "cubeSpaceMatrices[4]",
            "cubeSpaceMatrices[5]",
    };
    static std::string TextureName(int type, int n = 0);

};


extern Shader* VoidShader;

// forward shaders
extern Shader* SolidShader;
extern Shader* SimpleShader;
extern Shader* TransparentShader;
extern Shader* CutoutShader;
extern Shader* SkyShader;
extern Shader* SkyShaderHDR;
extern Shader* SkyShaderEquirectangular;
extern Shader* EnvDiffIrradianceShader;
extern Shader* EnvSpecPrefilterShader;
extern Shader* IntegrateBRDFShader;

extern Shader* DepthShader;
extern Shader* DepthCubeShader;

// screen shaders
extern Shader* ScreenShader;
extern Shader* ScreenShaderHDR;

// deferred shaders
extern Shader* GBufferShader;
extern Shader* DeferredShader;
extern Shader* DeferredPLightShader;
extern Shader* DeferredPLNoShadowShader;
extern Shader* SSAOShader;

// PBR shaders
extern Shader* PBRShader;


extern Shader* GaussianBlurShader;


void compileShaders();
const Shader* selectShader(RenderType type);
const Shader* selectPBRShader(RenderType type);
const Shader* selectCubeShader(RenderType type);
const Shader* selectGBufferShader(RenderType type);

#endif //ISLAND_SHADERS_H
