//
// Created by HXYP on 2023/4/17.
//

#ifndef ISLAND_ABSTRACT_MESH_HPP
#define ISLAND_ABSTRACT_MESH_HPP
#include <string>
#include <vector>
#include <initializer_list>
#include "defs.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "assimp/material.h"
#include "util/shaders.h"
#include "util/stb_image.h"
#include "util/texture_util.h"

template<class Vert, class Tex>
class AbstractMesh {
public:
    AbstractMesh() = default;
    AbstractMesh(const std::vector<Vert>& vertices, const std::vector<uint>& indices, const std::vector<Tex>& textures);
    [[nodiscard]] Long getBufferSize() const;
    [[nodiscard]] Long getIndicesSize() const;
//    AbstractMesh& addTexture(const Tex& texture);
    AbstractMesh& setTextures(const std::initializer_list<Tex>& textures);
    virtual void draw(const Shader& shader) const = 0;
//    void setEnvironmentMap(GLuint cubeTex);
//    void setShadowMap(GLuint shadowTex);
    void bind() const;
    void unbind() const;

protected:
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
//    GLuint envMap = 0;
//    GLuint shadowMap = 0;
    std::vector<Vert> vertices;
    std::vector<Tex> textures;
    std::vector<uint> indices;
    void setupMesh();
    virtual void setupVertexAttribs() = 0;
};

template<class Vert, class Tex>
AbstractMesh<Vert, Tex>::AbstractMesh(const std::vector<Vert> &vertices, const std::vector<uint>& indices,
                                      const std::vector<Tex> &textures) {
    this->vertices = vertices;
    this->textures = textures;
    this->indices = indices;
}

template<class Vert, class Tex>
Long AbstractMesh<Vert, Tex>::getBufferSize() const {
    return static_cast<Long>(this->vertices.size() * sizeof(Vert));
}

template<class Vert, class Tex>
Long AbstractMesh<Vert, Tex>::getIndicesSize() const {
    return static_cast<Long>(this->indices.size() * sizeof(uint));
}

//template<class Vert, class Tex>
//AbstractMesh<Vert, Tex> &AbstractMesh<Vert, Tex>::addTexture(const Tex &texture) {
//    this->textures.push_back(texture);
//    return *this;
//}

template<class Vert, class Tex>
AbstractMesh<Vert, Tex>& AbstractMesh<Vert, Tex>::setTextures(const std::initializer_list<Tex>& _textures) {
    this->textures = _textures;
    return *this;
}

template<class Vert, class Tex>
void AbstractMesh<Vert, Tex>::setupMesh() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->getBufferSize(), &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->getIndicesSize(), &this->indices[0], GL_STATIC_DRAW);

    this->setupVertexAttribs();
    // detach
    glBindVertexArray(0);

}


template<class Vert, class Tex>
void AbstractMesh<Vert, Tex>::bind() const {
    glBindVertexArray(this->VAO);
}

template<class Vert, class Tex>
void AbstractMesh<Vert, Tex>::unbind() const {
    glBindVertexArray(0);
}

#endif //ISLAND_ABSTRACT_MESH_HPP
