//
// Created by HXYP on 2023/4/7.
//

#include "util/Mesh.h"
#include "util/texture_util.h"

Mesh::Mesh(const std::vector<Vertex3D> &vertices, const std::vector<uint> &indices,
           const std::vector<Texture2D> &textures): AbstractMesh<Vertex3D, Texture2D>(vertices, indices, textures) {
    this->setupMesh();
}

Mesh::Mesh() = default;

void Mesh::draw(const Shader &shader) const {
    int nDiff = 0;
    int nSpec = 0;
    int nNorm = 0;
    int i = 0;
    for (const auto& tex : textures) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, tex.id);
        std::string name("material.");
        name.reserve(24);
        name += NameOfType(tex.type);
        switch (tex.type) {
            case aiTextureType_DIFFUSE:
                name += std::to_string(nDiff++);
                break;
            case aiTextureType_SPECULAR:
                name += std::to_string(nSpec++);
                break;
            case aiTextureType_NORMALS:
                name += std::to_string(nNorm++);
            default:
                break;
        }
        shader.use();
        shader.uniformInt(name, i);
        i += 1;
    }
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_CUBE_MAP, this->envMap);
    shader.uniformInt("environment", i);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int>(this->indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::setupVertexAttribs() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), (void*)offsetof(Vertex3D, uv));
}