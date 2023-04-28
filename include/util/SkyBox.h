//
// Created by HXYP on 2023/4/16.
//

#ifndef ISLAND_SKYBOX_H
#define ISLAND_SKYBOX_H
#include "AbstractMesh.hpp"
#include "texture_util.h"

class SkyBox: public AbstractMesh<VertexCube, GLuint> {
public:
    SkyBox() = default;
    SkyBox(const std::vector<VertexCube>& vertices, const std::vector<uint>& indices, const std::vector<GLuint>& textures);
    void draw(const Shader &shader) const override;
protected:
    void setupVertexAttribs() override;
};


#endif //ISLAND_SKYBOX_H
