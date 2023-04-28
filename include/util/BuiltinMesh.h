//
// Created by HXYP on 2023/4/12.
//

#ifndef ISLAND_BUILTIN_MODEL_H
#define ISLAND_BUILTIN_MODEL_H
#include "util/Mesh.h"


class BuiltinMesh : public Mesh {
public:
    BuiltinMesh& addVertex(float x, float y, float z);
    BuiltinMesh& setNormal(float x, float y, float z);
    BuiltinMesh& setNormal(const glm::vec3& normal);
    BuiltinMesh& setUV(float u, float v);
    BuiltinMesh& setFaceNormal(float x, float y, float z);
    BuiltinMesh& setFaceNormal(const glm::vec3& normal);
    BuiltinMesh& nextFace();
    void build();

protected:
    size_t faceHeader = 0;
};


#endif //ISLAND_BUILTIN_MODEL_H
