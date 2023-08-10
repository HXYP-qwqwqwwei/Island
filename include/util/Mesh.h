//
// Created by HXYP on 2023/4/7.
//

#ifndef ISLAND_MESH_H
#define ISLAND_MESH_H
#include <string>
#include <vector>
#include "defs.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include "assimp/material.h"
#include "shaders.h"
#include "stb_image.h"
#include "texture_util.h"
#include "AbstractMesh.hpp"

class Mesh: public AbstractMesh<Vertex3D, Texture2DWithType>{
    friend class Model;
public:
    Mesh(const std::vector<Vertex3D>& vertices, const std::vector<uint>& indices, const std::vector<Texture2DWithType>& textures);
    Mesh();
    void draw(const Shader& shader) const override;

protected:
    void drawInstanced(const Shader& shader, int amount) const;
    void setupVertexAttribs() override;

    void setupTextures(const Shader &shader) const;
};


#endif //ISLAND_MESH_H
