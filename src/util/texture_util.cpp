//
// Created by HXYP on 2023/4/15.
//

#include "util/texture_util.h"


GLuint load_texture(const char* path, const std::string& directory, GLint parm, bool flipUV, bool sRGB) {
    int width, height, nrChannels;
    std::string fullPath = directory + '/' + path;
    GLuint id;
    stbi_set_flip_vertically_on_load(flipUV);
    unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cerr << "ERROR::TEXTURE::Failed to load texture at \"" << fullPath << "\"\n";
        return 0;
    }
    auto formatTuple = tex_format(nrChannels);
    GLint format = get<RGB>(formatTuple);
    GLint internalFormat = sRGB ? get<SRGB>(formatTuple) : format;
    if (internalFormat == -1) return 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, parm);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, parm);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return id;
}


GLuint load_cube_map(std::initializer_list<std::string> paths, const std::string& directory, bool flipUV) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(flipUV);
    for (size_t i = 0; i < paths.size(); ++i) {
        std::string fullPath = directory + '/' + *(paths.begin() + i);
        unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
        if (data == nullptr) {
            std::cerr << "ERROR::TEXTURE::Failed to load texture at \"" << fullPath << "\"\n";
            return 0;
        }
        auto format = tex_format(nrChannels);
        if (get<SRGB>(format) == -1) {
            stbi_image_free(data);
            return 0;
        }
        glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, get<SRGB>(format), width, height, 0, get<RGB>(format), GL_UNSIGNED_BYTE, data
        );
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return id;
}

constexpr const std::tuple<glm::vec3, glm::vec3, glm::vec3> FOCAL_VECTORS[6] {
        {glm::vec3(-1, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0)},  // XP
        {glm::vec3(1, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)},  // XN
        {glm::vec3(0, -1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, 1)},  // YP
        {glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), glm::vec3(0, 0, -1)},  // YN
        {glm::vec3(0, 0, -1), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0)}, // ZP
        {glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0)},   // ZN
};

GLuint create_cube_map(Shader& shader, const glm::vec3& pos, GLuint fBuffer) {
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);
    const glm::mat4 model = glm::mat4(1);
    const glm::mat4 proj = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    for (uint8_t i = 0; i < 6; ++i) {
        Camera camera(FOCAL_VECTORS[i]);
        camera.moveTo(pos);
        shader.uniformMatrix4fv(shader::VIEW, camera.getView());
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    return id;
}



Texture2D load_texture(const char* path, const std::string& directory, aiTextureType type, GLint parm, bool flipUV) {
    GLuint id = load_texture(path, directory, parm, flipUV, false);
    return {
        id, type
    };
}


const char* NameOfType(int type) {
    switch (type) {
        case aiTextureType_DIFFUSE:
            return "texture_diffuse";
        case aiTextureType_SPECULAR:
            return "texture_specular";
        case aiTextureType_NORMALS:
            return "texture_normals";
        default:
            std::cerr << "WARN::TEXTURE::Unsupported texture type:" << type << '\n';
            return "";
    }
}


std::tuple<GLint, GLint> tex_format(int nrChannels) {
    switch (nrChannels) {
        case 1:
            return {GL_RED, GL_RED};
        case 3:
            return {GL_SRGB, GL_RGB};
        case 4:
            return {GL_SRGB_ALPHA, GL_RGBA};
        default:
            std::cerr << "ERROR::TEXTURE::Unsupported file format\n";
            return {-1, -1};
    }
}
