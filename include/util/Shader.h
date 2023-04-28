//
// Created by HXYP on 2023/3/7.
//

#ifndef ISLAND_SHADER_H
#define ISLAND_SHADER_H

#include <string>
#include <iostream>
#include <fstream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
using GLObject = GLuint;
using GLLoc = GLint;

namespace shader {
    constexpr const char MODEL[] = "model";
    constexpr const char VIEW[] = "view";
    constexpr const char PROJECTION[] = "proj";
}


class Shader {
private:
    const GLObject shaderProgram;

    static void compileShader(const std::string& source, GLObject& shaderObject, GLenum type) ;
    void attachShader(GLObject shader) const;

public:
    explicit Shader();
    void loadShader(const std::string& filename, GLenum type);
    void use() const;
    void link() const;
    void uniformMatrix4fv(const std::string& name, glm::mat4 matrix) const;
    void uniformVec3(const std::string& name, const glm::vec3& vec) const;
    void uniformVec4(const std::string& name, const glm::vec4& vec) const;
    void uniformFloat(const std::string& name, const float& fv) const;
    void uniformInt(const std::string& name, const int& iv) const;
};


#endif //ISLAND_SHADER_H
