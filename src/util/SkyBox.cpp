//
// Created by HXYP on 2023/4/16.
//

#include "util/SkyBox.h"

SkyBox::SkyBox(const std::vector<VertexCube> &vertices, const std::vector<uint> &indices,
               const std::vector<TextureCube> &textures) : AbstractMesh(vertices, indices, textures) {
    this->setupMesh();
}

void SkyBox::draw(const Shader &shader) const {
    int i = 0;
    for (auto tex : textures) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex.id);
        std::string name("texture");
        name.reserve(10);
        name += std::to_string(i);
        shader.use();
        shader.uniformInt(name, i);
        i += 1;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, static_cast<int>(this->indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

}

void SkyBox::setupVertexAttribs() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexCube), nullptr);
}
