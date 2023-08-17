//
// Created by HXYP on 2023/4/7.
//

#include "util/Mesh.h"
#include "util/texture_util.h"

Mesh::Mesh(const std::vector<Vertex3D> &vertices, const std::vector<uint> &indices,
           const std::vector<Texture2DWithType> &textures): AbstractMesh<Vertex3D, Texture2DWithType>(vertices, indices, textures) {
    this->setupMesh();
}

Mesh::Mesh() = default;

void Mesh::draw(const Shader &shader) const {
    this->setupTextures(shader);
    this->bind();
    glDrawElements(GL_TRIANGLES, static_cast<int>(this->indices.size()), GL_UNSIGNED_INT, nullptr);
    this->unbind();
}


void Mesh::drawInstanced(const Shader &shader, int amount) const {
    this->setupTextures(shader);
    this->bind();
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<int>(this->indices.size()), GL_UNSIGNED_INT, nullptr, amount);
    this->unbind();
}


void Mesh::setupTextures(const Shader &shader) const {
    std::unordered_map<aiTextureType, GLint> nTex;

    shader.use();
    shader.setDefaultTexture(aiTextureType_DIFFUSE,      textures::MISSING.id,       DEFAULT_TEXTURE_DIFF);
    shader.setDefaultTexture(aiTextureType_SPECULAR,     textures::BLACK_RGB.id,     DEFAULT_TEXTURE_SPEC);
    shader.setDefaultTexture(aiTextureType_NORMALS,      textures::FLAT_NORMALS.id,  DEFAULT_TEXTURE_NORM);
    shader.setDefaultTexture(aiTextureType_REFLECTION,   textures::BLACK_RGB.id,     DEFAULT_TEXTURE_REFL);
    shader.setDefaultTexture(aiTextureType_DISPLACEMENT, textures::FLAT_PARALLAX.id, DEFAULT_TEXTURE_PARA);
    shader.setDefaultTexture(aiTextureType_METALNESS,           textures::BLACK_GRAY.id,    DEFAULT_TEXTURE_METAL);
    shader.setDefaultTexture(aiTextureType_DIFFUSE_ROUGHNESS,   textures::WHITE_GRAY.id,    DEFAULT_TEXTURE_ROUGH);
    shader.setDefaultTexture(aiTextureType_AMBIENT_OCCLUSION,   textures::WHITE_GRAY.id,    DEFAULT_TEXTURE_AO);


    int i = 0;
    for (const auto& tex : textures) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        std::string name(Shader::TEXTURES);
        name.reserve(24);
        name += Shader::TextureName(tex.type, nTex[tex.type]++);
        shader.uniformInt(name, i);
        i += 1;
    }
    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, 0);
}


void Mesh::setupVertexAttribs() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, uv));

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, tangent));

}