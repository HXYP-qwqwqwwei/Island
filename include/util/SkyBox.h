//
// Created by HXYP on 2023/4/16.
//

#ifndef ISLAND_SKYBOX_H
#define ISLAND_SKYBOX_H
#include "AbstractMesh.hpp"
#include "texture_util.h"

class SkyBox: public AbstractMesh<glm::vec3, TextureCube> {
public:
    SkyBox() = default;
    SkyBox(const std::vector<glm::vec3>& vertices, const std::vector<uint>& indices, const std::vector<TextureCube>& textures);
    void draw(const Shader &shader) const override;
protected:
    void setupVertexAttribs() override;
};

class SkyBoxEquirectangular: public AbstractMesh<glm::vec3, Texture2D> {
public:
    SkyBoxEquirectangular() = default;
    SkyBoxEquirectangular(const std::vector<glm::vec3>& vertices, const std::vector<uint>& indices, const std::vector<Texture2D>& textures);
    void draw(const Shader &shader) const override;
protected:
    void setupVertexAttribs() override;
};



#endif //ISLAND_SKYBOX_H
