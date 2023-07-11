//
// Created by HXYP on 2023/4/15.
//

#ifndef ISLAND_TEXTURE_UTIL_H
#define ISLAND_TEXTURE_UTIL_H

#define SRGB 0
#define RGB 1

#include <vector>
#include <string>
#include <tuple>
#include "util/stb_image.h"
#include "util/shaders.h"
#include "util/Camera.h"
#include "assimp/material.h"
#include "glm/glm.hpp"
#include "glad/glad.h"
#include "assimp/defs.h"

struct Vertex2D {
    glm::vec2 position;
    glm::vec2 uv;
};

struct Vertex3D {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

struct Vertex3DNoTex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct VertexCube {
    glm::vec3 position;
};

struct Texture2D {
    GLuint id = 0;
    aiTextureType type = aiTextureType_DIFFUSE;
};


std::tuple<GLint, GLint> tex_format(int nrChannels);
GLuint load_texture(const char* path, const std::string& directory, GLint parm = GL_REPEAT, bool flipUV = false, bool sRGB = true);
GLuint load_cube_map(std::initializer_list<std::string> paths, const std::string& directory, bool flipUV = false);
Texture2D load_texture(const char* path, const std::string& directory, aiTextureType type, GLint parm = GL_REPEAT, bool flipUV = true);


#endif //ISLAND_TEXTURE_UTIL_H
