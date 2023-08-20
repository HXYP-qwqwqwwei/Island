//
// Created by HXYP on 2023/3/7.
//

#include "util/shaders.h"
#include "assimp/material.h"


Shader* VoidShader;

// forward shaders
Shader* SolidShader;
Shader* SimpleShader;
Shader* TransparentShader;
Shader* CutoutShader;
Shader* SkyShader;
Shader* SkyShaderHDR;
Shader* SkyShaderEquirectangular;
Shader* EnvDiffIrradianceShader;
Shader* EnvSpecPrefilterShader;
Shader* IntegrateBRDFShader;

Shader* DepthShader;
Shader* DepthCubeShader;

// screen shaders
Shader* ScreenShader;
Shader* ScreenShaderHDR;

// deferred shaders
Shader* GBufferShader;
Shader* DeferredShader;
Shader* DeferredPLightShader;
Shader* DeferredPLNoShadowShader;
Shader* SSAOShader;

// PBR shaders
Shader* PBRShader;


Shader* GaussianBlurShader;



void compileShaders() {
    VoidShader = new Shader();
    VoidShader->loadShader("VoidShader.vert", GL_VERTEX_SHADER);
    VoidShader->loadShader("VoidShader.frag", GL_FRAGMENT_SHADER);
    VoidShader->link();

    SolidShader = new Shader();
    SolidShader->loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    SolidShader->loadShader("SolidShader.frag", GL_FRAGMENT_SHADER);
    SolidShader->link();

    SimpleShader = new Shader();
    SimpleShader->loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    SimpleShader->loadShader("PureColorShader.frag", GL_FRAGMENT_SHADER);
    SimpleShader->link();

    TransparentShader = new Shader();
    TransparentShader->loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    TransparentShader->loadShader("TransparentShader.frag", GL_FRAGMENT_SHADER);
    TransparentShader->link();

    CutoutShader = new Shader();
    CutoutShader->loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    CutoutShader->loadShader("CutoutShader.frag", GL_FRAGMENT_SHADER);
    CutoutShader->link();

    ScreenShader = new Shader();
    ScreenShader->loadShader("ScreenShader.vert", GL_VERTEX_SHADER);
    ScreenShader->loadShader("ScreenShader.frag", GL_FRAGMENT_SHADER);
    ScreenShader->link();

    ScreenShaderHDR = new Shader();
    ScreenShaderHDR->loadShader("ScreenShader.vert", GL_VERTEX_SHADER);
    ScreenShaderHDR->loadShader("ScreenShaderHDR.frag", GL_FRAGMENT_SHADER);
    ScreenShaderHDR->link();

    GBufferShader = new Shader();
    GBufferShader->loadShader("GBufferShader.vert", GL_VERTEX_SHADER);
    GBufferShader->loadShader("GBufferShader.frag", GL_FRAGMENT_SHADER);
    GBufferShader->link();

    DeferredShader = new Shader();
    DeferredShader->loadShader("DeferredDLightShader.vert", GL_VERTEX_SHADER);
    DeferredShader->loadShader("DeferredDLightShader.frag", GL_FRAGMENT_SHADER);
    DeferredShader->link();

    DeferredPLightShader = new Shader();
    DeferredPLightShader->loadShader("DeferredPLightShader.vert", GL_VERTEX_SHADER);
    DeferredPLightShader->loadShader("DeferredPLightShader.frag", GL_FRAGMENT_SHADER);
    DeferredPLightShader->link();

    DeferredPLNoShadowShader = new Shader();
    DeferredPLNoShadowShader->loadShader("DeferredPLightNoShadow.vert", GL_VERTEX_SHADER);
    DeferredPLNoShadowShader->loadShader("DeferredPLightNoShadow.frag", GL_FRAGMENT_SHADER);
    DeferredPLNoShadowShader->link();

    SSAOShader = new Shader();
    SSAOShader->loadShader("ScreenShader.vert", GL_VERTEX_SHADER);
    SSAOShader->loadShader("SSAOShader.frag", GL_FRAGMENT_SHADER);
    SSAOShader->link();

    PBRShader = new Shader();
    PBRShader->loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    PBRShader->loadShader("PBRShader.frag", GL_FRAGMENT_SHADER);
    PBRShader->link();


    SkyShader = new Shader();
    SkyShader->loadShader("SkyShader.vert", GL_VERTEX_SHADER);
    SkyShader->loadShader("SkyShader.frag", GL_FRAGMENT_SHADER);
    SkyShader->link();

    SkyShaderHDR = new Shader();
    SkyShaderHDR->loadShader("SkyShader.vert", GL_VERTEX_SHADER);
    SkyShaderHDR->loadShader("SkyShaderHDR.frag", GL_FRAGMENT_SHADER);
    SkyShaderHDR->link();

    SkyShaderEquirectangular = new Shader();
    SkyShaderEquirectangular->loadShader("SkyShader.vert", GL_VERTEX_SHADER);
    SkyShaderEquirectangular->loadShader("SkyShaderEquirectangular.frag", GL_FRAGMENT_SHADER);
    SkyShaderEquirectangular->link();

    EnvDiffIrradianceShader = new Shader();
    EnvDiffIrradianceShader->loadShader("SkyShader.vert", GL_VERTEX_SHADER);
    EnvDiffIrradianceShader->loadShader("EnvDiffIrradianceShader.frag", GL_FRAGMENT_SHADER);
    EnvDiffIrradianceShader->link();

    EnvSpecPrefilterShader = new Shader();
    EnvSpecPrefilterShader->loadShader("SkyShader.vert", GL_VERTEX_SHADER);
    EnvSpecPrefilterShader->loadShader("EnvPrefilterShader.frag", GL_FRAGMENT_SHADER);
    EnvSpecPrefilterShader->link();

    IntegrateBRDFShader = new Shader();
    IntegrateBRDFShader->loadShader("ScreenShader.vert", GL_VERTEX_SHADER);
    IntegrateBRDFShader->loadShader("IntegrateBRDFShader.frag", GL_FRAGMENT_SHADER);
    IntegrateBRDFShader->link();


    DepthShader = new Shader();
    DepthShader->loadShader("DepthShader.vert", GL_VERTEX_SHADER);
    DepthShader->loadShader("DepthShader.frag", GL_FRAGMENT_SHADER);
    DepthShader->link();

    DepthCubeShader = new Shader();
    DepthCubeShader->loadShader("DepthCubeShader.vert", GL_VERTEX_SHADER);
    DepthCubeShader->loadShader("CubeShader.geom", GL_GEOMETRY_SHADER);
    DepthCubeShader->loadShader("DepthCubeShader.frag", GL_FRAGMENT_SHADER);
    DepthCubeShader->link();


    GaussianBlurShader = new Shader();
    GaussianBlurShader->loadShader("ScreenShader.vert", GL_VERTEX_SHADER);
    GaussianBlurShader->loadShader("GaussianBlur.frag", GL_FRAGMENT_SHADER);
    GaussianBlurShader->link();

}

bool Shader::compileShader(const std::string& source, GLObject& shaderObject, GLenum type) {
    int success = 1;
    char infoLog[512];
    shaderObject = glCreateShader(type);

    // 编译着色器
    const char* c_source = source.c_str();
    glShaderSource(shaderObject, 1, &c_source, nullptr);
    glCompileShader(shaderObject);
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderObject, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return success;
}

const Shader* selectShader(RenderType type) {
    switch (type) {
        case SOLID:
        case CUTOUT:
            return SolidShader;
//            return CutoutShader;
        case TRANSPARENT:
            return TransparentShader;
        case SCREEN:
            return ScreenShader;
        case SHADOW:
            return DepthShader;
        case PURE:
            return SimpleShader;
    }
}

const Shader* selectPBRShader(RenderType type) {
    return PBRShader;
}

const Shader* selectCubeShader(RenderType type) {
    switch (type) {
        case SOLID:
            return SolidShader;
        case CUTOUT:
            return CutoutShader;
        case TRANSPARENT:
            return TransparentShader;
        case SCREEN:
            return ScreenShader;
        case SHADOW:
            return DepthCubeShader;
        case PURE:
            return SimpleShader;
    }

}

const Shader *selectGBufferShader(RenderType type) {
    return GBufferShader;
}

std::string Shader::TextureName(int type, int n) {
    std::string name;
    name.reserve(20);
    switch (type) {
        case aiTextureType_DIFFUSE:
            name = "diffuse";
            break;
        case aiTextureType_SPECULAR:
            name = "specular";
            break;
        case aiTextureType_REFLECTION:
            name = "reflect";
            break;
        case aiTextureType_NORMALS:
            name = "normals";
            break;
        case aiTextureType_DISPLACEMENT:
            name = "parallax";
            break;
        case aiTextureType_DIFFUSE_ROUGHNESS:
            name = "roughness";
            break;
        case aiTextureType_METALNESS:
            name = "metalness";
            break;
        case aiTextureType_AMBIENT_OCCLUSION:
            name = "ao";
            break;
        default:
            std::cerr << "WARN::TEXTURE::Unsupported texture type:" << type << '\n';
            return "";
    }
    name += std::to_string(n);
    return name;
}




Shader::Shader(): shaderProgram(glCreateProgram()) {

}

void Shader::loadShader(const std::string& filename, GLenum type) {
    const std::string path = "Shader/" + filename;
//    const std::string path = filename;
    std::ifstream input(path);
    if (!input.is_open()) {
        std::cerr << "ERROR::SHADER::File \"" << filename << "\" Not Found.\n";
        return;
    }
    const std::string source((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    input.close();
    GLObject shader;
    if(!compileShader(source, shader, type)) {
        std::cerr << "File \"" << filename << "\" compile failed.\n";
        return;
    }
    this->attachShader(shader);
    glDeleteShader(shader);
}

void Shader::attachShader(GLObject shader) const {
    glAttachShader(this->shaderProgram, shader);
}

void Shader::use() const {
    glUseProgram(this->shaderProgram);
}

void Shader::link() const {
    glLinkProgram(this->shaderProgram);
}

void Shader::uniformMatrix4fv(const std::string &name, glm::mat4 matrix) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::uniformVec3(const std::string &name, const glm::vec3& vec) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform3fv(loc, 1, glm::value_ptr(vec));
}

void Shader::uniformVec2(const std::string &name, const glm::vec2& vec) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform2fv(loc, 1, glm::value_ptr(vec));
}

void Shader::uniformFloat(const std::string &name, float fv) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform1fv(loc, 1, &fv);
}

void Shader::uniformInt(const std::string &name, int iv) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform1iv(loc, 1, &iv);
}

void Shader::uniformUInt(const std::string& name, GLuint uiv) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform1uiv(loc, 1, &uiv);
}

void Shader::uniformBool(const std::string &name, bool bv) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform1i(loc, bv);
}

void Shader::setDefaultTexture(aiTextureType type, GLuint tex, int idx) const {
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, tex);
    this->uniformInt(Shader::TEXTURES + Shader::TextureName(type), idx);
}

void Shader::setEnvironmentMap(GLuint envMap) const {
    glActiveTexture(GL_TEXTURE0 + ENVIRONMENT_MAP_TEX);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);
    this->uniformInt(Shader::ENVIRONMENT_MAP, ENVIRONMENT_MAP_TEX);
}

void Shader::setEnvironmentDiffuse(GLuint envDiff) const {
    glActiveTexture(GL_TEXTURE0 + ENVIRONMENT_DIFFUSE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envDiff);
    this->uniformInt(Shader::ENVIRONMENT_IRR, ENVIRONMENT_DIFFUSE);
}

void Shader::setEnvironmentPrefiltered(GLuint envPrefiltered) const {
    glActiveTexture(GL_TEXTURE0 + ENVIRONMENT_PREFILTERED);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envPrefiltered);
    this->uniformInt(Shader::ENVIRONMENT_PF, ENVIRONMENT_PREFILTERED);
}

void Shader::setBRDFLookUpTex(GLuint brdfLUT) const {
    glActiveTexture(GL_TEXTURE0 + BRDF_LOOK_UP_TEXTURE);
    glBindTexture(GL_TEXTURE_2D, brdfLUT);
    this->uniformInt(Shader::BRDF_LUT, BRDF_LOOK_UP_TEXTURE);

}
