//
// Created by HXYP on 2023/4/12.
//

#ifndef ISLAND_SHAPES_H
#define ISLAND_SHAPES_H
#include <cstdarg>
#include <initializer_list>
#include "BuiltinMesh.h"
#include "Model.h"
#include "Screen.h"
#include "SkyBox.h"

template<class T>
using TexList = const std::initializer_list<T>&;

namespace shapes {
    Model Cube(float len, TexList<Texture2D> textures = {});
    Model Rectangle(float w, float h, TexList<Texture2D> textures = {}, float maxU = 1, float maxV = 1);
    Model Ball(float radius, int segmentsXZ, int segmentsY);
    Screen* ScreenRect(TexList<GLuint> tex = {});
    SkyBox* SkyBoxCube(GLuint texture);
}

#endif //ISLAND_SHAPES_H
