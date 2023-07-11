//
// Created by HXYP on 2023/3/7.
//

#ifndef ISLAND_SHADERS_H
#define ISLAND_SHADERS_H

#include <string>
#include <iostream>
#include <fstream>
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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
    PURE
};


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
    void uniformFloat(const std::string& name, float fv) const;
    void uniformInt(const std::string& name, int iv) const;

    /*===== Uniform names =====*/
    static constexpr const char MODEL[] = "model";
    static constexpr const char VIEW[] = "view";
    static constexpr const char VIEW_POS[] = "viewPos";
    static constexpr const char SHININESS[] = "texes.shininess";
    static constexpr const char TEXTURES[] = "texes.";
    static constexpr const char PROJECTION[] = "proj";
    static constexpr const char CUBE_SPACE_MATRICES[6][21] = {
            "cubeSpaceMatrices[0]",
            "cubeSpaceMatrices[1]",
            "cubeSpaceMatrices[2]",
            "cubeSpaceMatrices[3]",
            "cubeSpaceMatrices[4]",
            "cubeSpaceMatrices[5]",
    };
    static constexpr const char Z_FAR[] = "zFar";
    static const char* TextureName(int type);


};


extern Shader* solidShader;
extern Shader* simpleShader;
extern Shader* transparentShader;
extern Shader* cutoutShader;
extern Shader* screenShader;
extern Shader* skyShader;
//extern Shader* instancedShader;
extern Shader* depthShader;
extern Shader* depthCubeShader;


void compileShaders();
const Shader* selectShader(RenderType type);
const Shader* selectCubeShader(RenderType type);

#endif //ISLAND_SHADERS_H
