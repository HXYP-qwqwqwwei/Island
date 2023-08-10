//
// Created by HXYP on 2023/4/15.
//

#include "util/Screen.h"

Screen::Screen(const std::vector<Vertex2D> &vertices, const std::vector<uint> &indices,
               const std::vector<Texture2D> &textures): AbstractMesh<Vertex2D, Texture2D>(vertices, indices, textures) {
    this->setupMesh();
}


Screen::Screen() = default;


void Screen::draw(const Shader &shader) const {
    int i = 0;
    for (auto tex : textures) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, tex.id);
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


void Screen::setupVertexAttribs() {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, uv));
}

