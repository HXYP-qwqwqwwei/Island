//
// Created by HXYP on 2023/3/7.
//

#include "util/shaders.h"
#include "assimp/material.h"

Shader* solidShader;
Shader* simpleShader;
Shader* transparentShader;
Shader* cutoutShader;
Shader* screenShader;
Shader* screenShaderHDR;
Shader* skyShader;
Shader* depthShader;
Shader* depthCubeShader;




void compileShaders() {
    solidShader = new Shader();
    solidShader->loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    solidShader->loadShader("SolidShader.frag", GL_FRAGMENT_SHADER);
    solidShader->link();


    simpleShader = new Shader();
    simpleShader->loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    simpleShader->loadShader("SimpleShader.frag", GL_FRAGMENT_SHADER);
    simpleShader->link();

    transparentShader = new Shader();
    transparentShader->loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    transparentShader->loadShader("TransparentShader.frag", GL_FRAGMENT_SHADER);
    transparentShader->link();

    cutoutShader = new Shader();
    cutoutShader->loadShader("CompletedShader.vert", GL_VERTEX_SHADER);
    cutoutShader->loadShader("CutoutShader.frag", GL_FRAGMENT_SHADER);
    cutoutShader->link();

    screenShader = new Shader();
    screenShader->loadShader("ScreenShader.vert", GL_VERTEX_SHADER);
    screenShader->loadShader("ScreenShader.frag", GL_FRAGMENT_SHADER);
    screenShader->link();

    screenShaderHDR = new Shader();
    screenShaderHDR->loadShader("ScreenShader.vert", GL_VERTEX_SHADER);
    screenShaderHDR->loadShader("ScreenShaderHDR.frag", GL_FRAGMENT_SHADER);
    screenShaderHDR->link();


    skyShader = new Shader();
    skyShader->loadShader("SkyShader.vert", GL_VERTEX_SHADER);
    skyShader->loadShader("SkyShader.frag", GL_FRAGMENT_SHADER);
    skyShader->link();

    depthShader = new Shader();
    depthShader->loadShader("DepthShader.vert", GL_VERTEX_SHADER);
    depthShader->loadShader("DepthShader.frag", GL_FRAGMENT_SHADER);
    depthShader->link();

    depthCubeShader = new Shader();
    depthCubeShader->loadShader("DepthCubeShader.vert", GL_VERTEX_SHADER);
    depthCubeShader->loadShader("CubeShader.geom", GL_GEOMETRY_SHADER);
    depthCubeShader->loadShader("DepthCubeShader.frag", GL_FRAGMENT_SHADER);
    depthCubeShader->link();
}

void Shader::compileShader(const std::string& source, GLObject& shaderObject, GLenum type) {
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
}

const Shader* selectShader(RenderType type) {
    switch (type) {
        case SOLID:
            return solidShader;
        case CUTOUT:
            return cutoutShader;
        case TRANSPARENT:
            return transparentShader;
        case SCREEN:
            return screenShader;
        case SHADOW:
            return depthShader;
        case PURE:
            return simpleShader;
    }
}

const Shader* selectCubeShader(RenderType type) {
    switch (type) {
        case SOLID:
            return solidShader;
        case CUTOUT:
            return cutoutShader;
        case TRANSPARENT:
            return transparentShader;
        case SCREEN:
            return screenShader;
        case SHADOW:
            return depthCubeShader;
        case PURE:
            return simpleShader;
    }

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
    compileShader(source, shader, type);
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

void Shader::uniformVec4(const std::string &name, const glm::vec4& vec) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform4fv(loc, 1, glm::value_ptr(vec));
}

void Shader::uniformFloat(const std::string &name, float fv) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform1fv(loc, 1, &fv);
}

void Shader::uniformInt(const std::string &name, int iv) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform1iv(loc, 1, &iv);
}

void Shader::setDefaultTexture(aiTextureType type, GLuint tex, int idx) const {
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, tex);
    this->uniformInt(Shader::TEXTURES + Shader::TextureName(type), idx);
}

void Shader::setEnvironmentMap(GLuint envMap) const {
    glActiveTexture(GL_TEXTURE29);
    glBindTexture(GL_TEXTURE_CUBE_MAP, envMap);
    this->uniformInt(Shader::ENVIRONMENT_MAP, 29);
}
