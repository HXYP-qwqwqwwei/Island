//
// Created by HXYP on 2023/3/23.
//

#ifndef ISLAND_MATERIALS_H
#define ISLAND_MATERIALS_H
#include <tuple>
#include "glm/glm.hpp"
#include "shaders.h"

#define AMBIENT     0
#define DIFFUSE     1
#define SPECULAR    2
#define SHININESS   3

#define S_AMBIENT     "material.ambient"
#define S_DIFFUSE     "material.diffuse"
#define S_SPECULAR    "material.specular"
#define S_SHININESS   "material.shininess"


using Material = std::tuple<glm::vec3, glm::vec3, glm::vec3, float>;

void setShaderMaterial(Shader& shader, Material material);

namespace materials {
    Material of(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float shininess);
    Material of(glm::vec3 ambient, float diffuse, float specular, float shininess);
    Material of(float ambient, float diffuse, float specular, float shininess);
    Material of(float diffuse, float specular, float shininess);

    extern Material METAL;
    extern Material GOLD;
    extern Material EMERALD;
    extern Material PERL;
}

#endif //ISLAND_MATERIALS_H
