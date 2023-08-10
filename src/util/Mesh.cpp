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
    int nDiff = 0;
    int nSpec = 0;
    int nNorm = 0;
    int nRefl = 0;
    int nPara = 0;

    shader.use();
    shader.setDefaultTexture(aiTextureType_DIFFUSE,      textures::MISSING.id,       28);
    shader.setDefaultTexture(aiTextureType_SPECULAR,     textures::BLACK_RGB.id,     27);
    shader.setDefaultTexture(aiTextureType_NORMALS,      textures::FLAT_NORMALS.id,  26);
    shader.setDefaultTexture(aiTextureType_REFLECTION,   textures::BLACK_RGB.id,     25);
    shader.setDefaultTexture(aiTextureType_DISPLACEMENT, textures::FLAT_PARALLAX.id, 24);

    int i = 0;
    for (const auto& tex : textures) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        std::string name(Shader::TEXTURES);
        name.reserve(24);
        switch (tex.type) {
            case aiTextureType_DIFFUSE:
                name += Shader::TextureName(tex.type, nDiff++);
                break;
            case aiTextureType_SPECULAR:
                name += Shader::TextureName(tex.type, nSpec++);
                break;
            case aiTextureType_NORMALS:
                name += Shader::TextureName(tex.type, nNorm++);
                break;
            case aiTextureType_REFLECTION:
                name += Shader::TextureName(tex.type, nRefl++);
                break;
            case aiTextureType_DISPLACEMENT:
                name += Shader::TextureName(tex.type, nPara++);
            default:
                break;
        }
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