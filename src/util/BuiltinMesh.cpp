//
// Created by HXYP on 2023/4/12.
//

#include "util/BuiltinMesh.h"

BuiltinMesh &BuiltinMesh::addVertex(float x, float y, float z, float u, float v) {
    auto& vertices = this->vertices;
    auto& indices = this->indices;
    size_t id = vertices.size();

    Vertex3D curr{};
    curr.position = glm::vec3(x, y, z);
    curr.uv = glm::vec2(u, v);

    size_t nSuccessor = id - this->faceHeader;  // 前面的顶点数
    if (nSuccessor >= 2) {
        auto& header = vertices[this->faceHeader];
        auto& prev = vertices[id-1];

        glm::vec3 v1 = prev.position - header.position;
        glm::vec3 v2 = curr.position - header.position;
        glm::vec2 dUV1 = prev.uv - header.uv;
        glm::vec2 dUV2 = curr.uv - prev.uv;
        glm::vec3 E1 = prev.position - header.position;
        glm::vec3 E2 = curr.position - prev.position;
        // TBN matrix
        glm::vec3 N = glm::cross(v1, v2);
        glm::mat2x3 TB = glm::mat2x3(E1, E2) * glm::inverse(glm::mat2(glm::vec2(dUV1.x, dUV1.y), glm::vec2(dUV2.x, dUV2.y)));
        if (nSuccessor == 2) {
            prev.normal = header.normal = N;
            prev.tangent = header.tangent = TB[0];
        }
        curr.normal = N;
        curr.tangent = TB[0];

        // Set indices
        indices.push_back(this->faceHeader);
        indices.push_back(id-1);
        indices.push_back(id);
    }
    vertices.push_back(curr);
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

Mesh BuiltinMesh::build() {
    this->setupMesh();
    return {this->vertices, this->indices, this->textures};
}



