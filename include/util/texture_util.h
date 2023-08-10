//
// Created by HXYP on 2023/4/15.
//

#ifndef ISLAND_TEXTURE_UTIL_H
#define ISLAND_TEXTURE_UTIL_H

#include <vector>
#include <string>
#include <tuple>
#include "util/stb_image.h"
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
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec3 tangent;
};

struct VertexCube {
    glm::vec3 position;
};

struct TextureCube {
    GLuint id = 0;
    GLsizei width = 4;
    GLsizei height = 4;
    GLint internalFormat = GL_RGB;
};

struct Texture2D {
    GLuint id = 0;
    GLsizei width = 4;
    GLsizei height = 4;
    GLint internalFormat = GL_RGB;
};

struct Texture2DWithType: public Texture2D{
    aiTextureType type = aiTextureType_DIFFUSE;
};

namespace textures {
    extern Texture2D MISSING;
    extern Texture2D BLACK_RGB;
    extern Texture2D WHITE_RGB;
    extern Texture2D BLACK_GRAY;
    extern Texture2D WHITE_GRAY;
    extern Texture2D FLAT_NORMALS;
    extern Texture2D FLAT_PARALLAX;
    extern Texture2D NO_DIRECT_SHADOW;
    extern TextureCube EMPTY_ENV_MAP;
    extern TextureCube NO_POINT_SHADOW;

    void loadDefaultTextures(const std::string& dir);
}

Texture2D createTexture2D(GLint format, GLint internalFormat, GLenum type, int width, int height, const void *data, GLint warp,
                       GLint filter, bool genMipmap);
GLint tex_format(int nrChannels);
Texture2D load_texture(const char* path, const std::string& directory, GLint warp = GL_REPEAT, GLint filter = GL_LINEAR, bool flipUV = true);
TextureCube load_cube_map(std::initializer_list<std::string> paths, const std::string& directory, bool flipUV = false);
Texture2DWithType load_texture(const char* path, const std::string& directory, aiTextureType type, GLint warp = GL_REPEAT, GLint filter = GL_LINEAR, bool flipUV = true);


#endif //ISLAND_TEXTURE_UTIL_H
