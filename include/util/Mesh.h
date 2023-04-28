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
#include "Shader.h"
#include "stb_image.h"
#include "texture_util.h"
#include "AbstractMesh.hpp"

class Mesh: public AbstractMesh<Vertex3D, Texture2D>{
public:
    Mesh(const std::vector<Vertex3D>& vertices, const std::vector<uint>& indices, const std::vector<Texture2D>& textures);
    Mesh();
    void draw(const Shader& shader) const override;

protected:
    void setupVertexAttribs() override;
};


#endif //ISLAND_MESH_H
