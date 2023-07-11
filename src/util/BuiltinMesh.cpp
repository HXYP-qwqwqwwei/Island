//
// Created by HXYP on 2023/4/12.
//

#include "util/BuiltinMesh.h"

BuiltinMesh &BuiltinMesh::addVertex(float x, float y, float z) {
    auto& vertices = this->vertices;
    auto& indices = this->indices;
    size_t id = vertices.size();
    vertices.push_back({
        glm::vec3(x, y, z),
        glm::vec3(0, 0, 1),
        glm::vec2(0, 0)
    });
    if (id - this->faceHeader >= 2) {
        indices.push_back(this->faceHeader);
        indices.push_back(id-1);
        indices.push_back(id);
    }
    return *this;
}

BuiltinMesh &BuiltinMesh::setNormal(float x, float y, float z) {
    return this->setNormal(glm::vec3(x, y, z));
}

BuiltinMesh &BuiltinMesh::setNormal(const glm::vec3 &normal) {
    auto vertex = this->vertices.rbegin();
    vertex->normal = glm::normalize(normal);
    return *this;
}

BuiltinMesh &BuiltinMesh::nextFace() {
    this->faceHeader = this->vertices.size();
    return *this;
}

BuiltinMesh &BuiltinMesh::setUV(float u, float v) {
    auto vertex = this->vertices.rbegin();
    vertex->uv = glm::vec2(u, v);
    return *this;
}

BuiltinMesh &BuiltinMesh::setFaceNormal(float x, float y, float z) {
    return this->setFaceNormal(glm::vec3(x, y, z));
}

BuiltinMesh &BuiltinMesh::setFaceNormal(const glm::vec3 &normal) {
    size_t sz = this->vertices.size();
    glm::vec3 norm = glm::normalize(normal);
    for (size_t i = faceHeader; i < sz; ++i) {
        this->vertices[i].normal = norm;
    }
    return *this;
}

Mesh BuiltinMesh::build() {
    this->setupMesh();
    return {this->vertices, this->indices, this->textures};
}



