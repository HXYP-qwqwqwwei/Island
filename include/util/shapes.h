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
    Model Cube(float len, TexList<Texture2DWithType> textures = {});
    Model Rectangle(float w, float h, TexList<Texture2DWithType> textures = {}, float maxU = 1, float maxV = 1);
    BuiltinMesh* BallMesh(GLfloat radius, GLsizei segmentsXZ, GLsizei segmentsY, GLfloat maxU = 1.0f, GLfloat maxV = 1.0f);
    Model Ball(GLfloat radius, GLsizei segmentsXZ, GLsizei segmentsY, TexList<Texture2DWithType> textures = {}, GLfloat maxU = 1.0f, GLfloat maxV = 1.0f);
    Screen* ScreenRect(TexList<Texture2D> tex = {});
    SkyBox* SkyBoxCube(TextureCube texture = {});
    SkyBoxEquirectangular* SkyBoxCubeEquirectangular(Texture2D texture = {});
}

#endif //ISLAND_SHAPES_H
