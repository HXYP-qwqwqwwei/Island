//
// Created by HXYP on 2023/4/12.
//

#ifndef ISLAND_SHAPES_H
#define ISLAND_SHAPES_H
#include <cstdarg>
#include "BuiltinMesh.h"
#include "Screen.h"
#include "SkyBox.h"

namespace shapes {
    Mesh* Cube(float len);
    Mesh* Rectangle(float w, float h, float maxU = 1, float maxV = 1);
    Mesh* Ball(float radius, int segmentsXZ, int segmentsY);
    Screen* ScreenRect(std::initializer_list<GLuint> tex);
    SkyBox* SkyBoxCube(GLuint texture);
}

#endif //ISLAND_SHAPES_H
