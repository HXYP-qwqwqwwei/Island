//
// Created by HXYP on 2023/4/12.
//

#include "util/BuiltinMesh.h"

BuiltinMesh &BuiltinMesh::addVertex(float x, float y, float z, float u, float v) {
    auto& vertices = this->vertices;
    auto& indices = this->indices;
    size_t id = vertices.size();
    // TODO
    vertices.push_back({
        glm::vec3(x, y, z),
        glm::vec2(u, v),
    });
    size_t nVtx = id - this->faceHeader;
    if (nVtx >= 2) {
        auto& header = vertices[this->faceHeader];
        auto& prev = vertices[id-1];
        auto& curr = vertices[id];
        glm::vec3 v1 = prev.position - header.position;
        glm::vec3 v2 = curr.position - header.position;
        glm::vec3 norm = glm::cross(v1, v2);
        glm::vec2 dUV1 = prev.uv - header.uv;
        glm::vec2 dUV2 = curr.uv - prev.uv;
        glm::vec3 E1 = prev.position - header.position;
        glm::vec3 E2 = curr.position - prev.position;
        glm::mat2x3 TB = glm::mat2x3(E1, E2) * glm::inverse(glm::mat2(glm::vec2(dUV1.x, dUV1.y), glm::vec2(dUV2.x, dUV2.y)));
        if (nVtx == 2) {
            prev.normal = header.normal = norm;
            prev.tangent = header.tangent = TB[0];
            prev.bitangent = header.bitangent = TB[1];
        }
        curr.normal = norm;
        curr.tangent = TB[0];
        curr.bitangent = TB[1];

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

//BuiltinMesh &BuiltinMesh::setUV(float u, float v) {
//    auto vertex = this->vertices.rbegin();
//    vertex->uv = glm::vec2(u, v);
//    return *this;
//}

//BuiltinMesh &BuiltinMesh::setFaceNormal(float x, float y, float z) {
//    return this->setFaceNormal(glm::vec3(x, y, z));
//}

//BuiltinMesh &BuiltinMesh::setFaceNormal(const glm::vec3 &normal) {
//    size_t sz = this->vertices.size();
//    glm::vec3 norm = glm::normalize(normal);
//    for (size_t i = faceHeader; i < sz; ++i) {
//        this->vertices[i].normal = norm;
//    }
//    return *this;
//}

Mesh BuiltinMesh::build() {
    this->setupMesh();
    return {this->vertices, this->indices, this->textures};
}



