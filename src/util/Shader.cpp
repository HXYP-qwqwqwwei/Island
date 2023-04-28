//
// Created by HXYP on 2023/3/7.
//

#include "util/Shader.h"

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

Shader::Shader(): shaderProgram(glCreateProgram()) {

}

void Shader::loadShader(const std::string& filename, GLenum type) {
    const std::string path = "shader/" + filename;
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

void Shader::uniformFloat(const std::string &name, const float &fv) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform1fv(loc, 1, &fv);
}

void Shader::uniformInt(const std::string &name, const int &iv) const {
    GLLoc loc = glGetUniformLocation(shaderProgram, name.c_str());
    glUniform1iv(loc, 1, &iv);
}
