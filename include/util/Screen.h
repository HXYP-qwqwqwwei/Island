//
// Created by HXYP on 2023/4/15.
//

#ifndef ISLAND_SCREEN_H
#define ISLAND_SCREEN_H
#include <vector>
#include "defs.h"
#include "util/texture_util.h"
#include "util/shaders.h"
#include "Mesh.h"

class Screen: public AbstractMesh<Vertex2D, Texture2D>{
public:
    Screen(const std::vector<Vertex2D>& vertices, const std::vector<uint>& indices, const std::vector<Texture2D>& textures);
    Screen();
    void draw(const Shader& shader) const override;

protected:
    void setupVertexAttribs() override;
};


#endif //ISLAND_SCREEN_H
