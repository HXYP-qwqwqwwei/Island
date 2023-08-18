//
// Created by HXYP on 2023/4/15.
//

#include "util/texture_util.h"

Texture2D textures::MISSING;
Texture2D textures::BLACK_RGB;
Texture2D textures::WHITE_RGB;
Texture2D textures::BLACK_GRAY;
Texture2D textures::WHITE_GRAY;
Texture2D textures::FLAT_NORMALS;
Texture2D textures::FLAT_PARALLAX;
Texture2D textures::NO_DIRECT_SHADOW;
TextureCube textures::EMPTY_ENV_MAP;
TextureCube textures::NO_POINT_SHADOW;



void textures::loadDefaultTextures(const std::string& dir) {
    MISSING = load_texture("missing.png", dir, GL_REPEAT, GL_NEAREST);
    BLACK_RGB = load_texture("pure_black.png", dir);
    WHITE_RGB = load_texture("pure_white.png", dir);
    BLACK_GRAY = load_texture("pure_black_gray.png", dir);
    WHITE_GRAY = load_texture("pure_white_gray.png", dir);
    FLAT_NORMALS = load_texture("flat_normals.png", dir);
    FLAT_PARALLAX = WHITE_RGB;

    EMPTY_ENV_MAP = load_cube_map(
            {"pure_black.png", "pure_black.png", "pure_black.png", "pure_black.png", "pure_black.png", "pure_black.png"},
            dir
    );

    NO_POINT_SHADOW = load_cube_map(
            {"pure_white_gray.png", "pure_white_gray.png", "pure_white_gray.png", "pure_white_gray.png", "pure_white_gray.png", "pure_white_gray.png"},
            dir
    );

    NO_DIRECT_SHADOW = WHITE_GRAY;

}



Texture2D load_texture(const char* path, const std::string& directory, GLint warp, GLint filter, bool flipUV) {
    int width, height, nrChannels;
    std::string fullPath = directory + '/' + path;
    stbi_set_flip_vertically_on_load(flipUV);
    void* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cerr << "ERROR::TEXTURE::Failed to load texture \"" << fullPath << "\"\n";
        return {};
    }
    GLint format = tex_format(nrChannels);
    if (format == -1) {
        stbi_image_free(data);
        std::cerr << "ERROR::TEXTURE::Unsupported file format.\n";
        std::cerr << "    Failed to load HDR texture \"" << fullPath << "\"\n";
        return {};
    }
    auto tex = createTexture2D(format, format, GL_UNSIGNED_BYTE, width, height, data, warp, filter, true);

    stbi_image_free(data);
    return tex;
}


Texture2D load_texture_HDR(const char *path, const std::string &directory, GLint warp, GLint filter, bool flipUV) {
    int width, height, nrChannels;
    std::string fullPath = directory + '/' + path;
    stbi_set_flip_vertically_on_load(flipUV);
    void* data = stbi_loadf(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cerr << "ERROR::TEXTURE::Failed to load HDR texture \"" << fullPath << "\"\n";
        return {};
    }
    GLint format = tex_format(nrChannels);
    GLint internalFormat = tex_format_f(nrChannels);
    if (format == -1) {
        stbi_image_free(data);
        std::cerr << "ERROR::TEXTURE::Unsupported file format.\n";
        std::cerr << "    Failed to load HDR texture \"" << fullPath << "\"\n";
        return {};
    }
    auto tex = createTexture2D(format, internalFormat, GL_FLOAT, width, height, data, warp, filter, true);
    stbi_image_free(data);
    return tex;
}


Texture2D createTexture2D(GLint format, GLint internalFormat, GLenum type, int width, int height, const void *data, GLint warp,
                       GLint filter, bool genMipmap) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);

    if (genMipmap)
        glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    return {id, width, height, internalFormat};
}

Texture2D createTexture2D(GLint internalFormat, int width, int height, GLint warp, GLint filter) {
    return createTexture2D(GL_RGB, internalFormat, GL_UNSIGNED_BYTE, width, height, nullptr, warp, filter, false);
}

TextureCube createTextureCube(GLint internalFormat, int length, GLint warp, GLint filter) {
    GLObject tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, length, length, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, warp);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, warp);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, warp);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filter);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return {tex, length, internalFormat, warp, filter};
}


TextureCube load_cube_map(std::initializer_list<std::string> paths, const std::string& directory, bool flipUV) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(flipUV);
    GLint format;
    for (size_t i = 0; i < paths.size(); ++i) {
        std::string fullPath = directory + '/' + *(paths.begin() + i);
        unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
        if (data == nullptr) {
            std::cerr << "ERROR::TEXTURE::Failed to load texture at \"" << fullPath << "\"\n";
            return {};
        }
        format = tex_format(nrChannels);
        if (format == -1) {
            stbi_image_free(data);
            return {};
        }
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data
        );
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return {id, width, height, format};
}

constexpr const std::tuple<glm::vec3, glm::vec3, glm::vec3> FOCAL_VECTORS[6] {
        {glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)},  // XP
        {glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)},  // XN
        {glm::vec3(0, -1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)},  // YP
        {glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)},  // YN
        {glm::vec3(0, 0, -1), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)}, // ZP
        {glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)},   // ZN
};


Texture2DWithType load_texture(const char* path, const std::string& directory, aiTextureType type, GLint warp, GLint filter, bool flipUV) {
    Texture2D tex = load_texture(path, directory, warp, filter, flipUV);
    return {
        tex, type
    };
}



GLint tex_format(int nrChannels) {
    switch (nrChannels) {
        case 1:
            return GL_RED;
        case 2:
            return GL_RG;
        case 3:
            return GL_RGB;
        case 4:
            return GL_RGBA;
        default:
            return -1;
    }
}

GLint tex_format_f(int nrChannels) {
    switch (nrChannels) {
        case 1:
            return GL_R16F;
        case 2:
            return GL_RG16F;
        case 3:
            return GL_RGB16F;
        case 4:
            return GL_RGBA16F;
        default:
            return -1;
    }
}

