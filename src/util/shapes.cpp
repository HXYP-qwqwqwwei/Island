//
// Created by HXYP on 2023/4/12.
//

#include "util/shapes.h"

Model shapes::Cube(float len, TexList<Texture2D> textures) {
    BuiltinMesh cube;
    float offset = len/2;
    cube.addVertex(offset, offset, offset, 1.0f, 1.0f)
            .addVertex( -offset,  offset,   offset, 0.0f, 1.0f)
            .addVertex(-offset,  -offset,   offset, 0.0f, 0.0f)
            .addVertex(offset,   -offset,   offset, 1.0f, 0.0f)
            .nextFace()
            .addVertex(-offset,   offset,  -offset, 1.0f, 1.0f)
            .addVertex(offset,  offset,  -offset, 0.0f, 1.0f)
            .addVertex( offset,  -offset,  -offset, 0.0f, 0.0f)
            .addVertex( -offset,   -offset,  -offset, 1.0f, 0.0f)
            .nextFace()
            .addVertex( offset,   offset,  -offset, 1.0f, 1.0f)
            .addVertex( -offset,   offset,   -offset, 0.0f, 1.0f)
            .addVertex(-offset,   offset,   offset, 0.0f, 0.0f)
            .addVertex(offset,   offset,  offset, 1.0f, 0.0f)
            .nextFace()
            .addVertex(-offset,  -offset,  -offset, 1.0f, 1.0f)
            .addVertex(offset,  -offset,   -offset, 0.0f, 1.0f)
            .addVertex( offset,  -offset,   offset, 0.0f, 0.0f)
            .addVertex( -offset,  -offset,  offset, 1.0f, 0.0f)
            .nextFace()
            .addVertex( offset,   offset,  -offset, 1.0f, 1.0f)
            .addVertex( offset,  offset,  offset, 0.0f, 1.0f)
            .addVertex( offset,  -offset,   offset, 0.0f, 0.0f)
            .addVertex( offset,   -offset,   -offset, 1.0f, 0.0f)
            .nextFace()
            .addVertex(-offset,   offset,   offset, 1.0f, 1.0f)
            .addVertex(-offset,  offset,   -offset, 0.0f, 1.0f)
            .addVertex(-offset,  -offset,  -offset, 0.0f, 0.0f)
            .addVertex(-offset,   -offset,  offset, 1.0f, 0.0f);
    cube.setTextures(textures);
    return Model({cube.build()});
}

Model shapes::Rectangle(float w, float h, TexList<Texture2D> textures, float maxU, float maxV) {
    BuiltinMesh rect;
    float w_offset = w/2;
    float h_offset = h/2;
    rect.addVertex(    w_offset,    h_offset,   0, maxU, maxV)
            .addVertex(-w_offset,    h_offset,   0, 0,    maxV)
            .addVertex(-w_offset,   -h_offset,   0, 0,    0)
            .addVertex( w_offset,   -h_offset,   0, maxU, 0);
    rect.setTextures(textures);
    return Model({rect.build()});
}

//Mesh* shapes::Ball(float radius, int segmentsXZ, int segmentsY) {
//    std::vector<Vertex3DNoTex> vertices;
//    std::vector<uint> indices;
//    float dAlpha = AI_MATH_PI / segmentsY;
//    float dBeta = AI_MATH_TWO_PI / segmentsXZ;
//    for (int i = 1; i < segmentsY; ++i) {
//        for (int j = 1; j < segmentsXZ; ++j) {
//            float alpha = dAlpha * (float)i;
//            float beta = dBeta * (float)j;
//            float y = radius * glm::cos(alpha);
//            float r = radius * glm::sin(alpha);
//            float x = r * glm::sin(beta);
//            float z = r * glm::cos(beta);
//            vertices.push_back({
//                glm::vec3(x, y, z),
//                glm::vec3(x, y, z)
//            });
//        }
//    }
//}


Screen* shapes::ScreenRect(std::initializer_list<GLuint> tex) {
    std::vector<GLuint> textures(tex);
    std::vector<Vertex2D> vertices{
            {glm::vec2(1, 1), glm::vec2(1, 1)},
            {glm::vec2(-1, 1), glm::vec2(0, 1)},
            {glm::vec2(-1, -1), glm::vec2(0, 0)},
            {glm::vec2(1, -1), glm::vec2(1, 0)},
    };
    std::vector<uint> indices {
        0, 1, 2,
        0, 2, 3
    };
    auto* rect = new Screen(vertices, indices, textures);
    return rect;
}

SkyBox* shapes::SkyBoxCube(GLuint texture) {
    std::vector<VertexCube> vertices {
            {glm::vec3(  .5f,    .5f,    .5f)},
            {glm::vec3(  .5f,   -.5f,    .5f)},
            {glm::vec3( -.5f,   -.5f,    .5f)},
            {glm::vec3( -.5f,    .5f,    .5f)},

            {glm::vec3(  .5f,    .5f,   -.5f)},
            {glm::vec3( -.5f,    .5f,   -.5f)},
            {glm::vec3( -.5f,   -.5f,   -.5f)},
            {glm::vec3(  .5f,   -.5f,   -.5f)},

            {glm::vec3(  .5f,    .5f,    .5f)},
            {glm::vec3( -.5f,    .5f,    .5f)},
            {glm::vec3( -.5f,    .5f,   -.5f)},
            {glm::vec3(  .5f,    .5f,   -.5f)},

            {glm::vec3(  .5f,   -.5f,    .5f)},
            {glm::vec3(  .5f,   -.5f,   -.5f)},
            {glm::vec3( -.5f,   -.5f,   -.5f)},
            {glm::vec3( -.5f,   -.5f,    .5f)},

            {glm::vec3(  .5f,    .5f,    .5f)},
            {glm::vec3(  .5f,    .5f,   -.5f)},
            {glm::vec3(  .5f,   -.5f,   -.5f)},
            {glm::vec3(  .5f,   -.5f,    .5f)},

            {glm::vec3( -.5f,    .5f,    .5f)},
            {glm::vec3( -.5f,   -.5f,    .5f)},
            {glm::vec3( -.5f,   -.5f,   -.5f)},
            {glm::vec3( -.5f,    .5f,   -.5f)},
    };

    std::vector<uint> indices {
            0,  1,  2,  0,  2,  3,
            4,  5,  6,  4,  6,  7,
            8,  9,  10, 8,  10, 11,
            12, 13, 14, 12, 14, 15,
            16, 17, 18, 16, 18, 19,
            20, 21, 22, 20, 22, 23
    };

    std::vector<GLuint> textures = {texture};
    return new SkyBox(vertices, indices, textures);
}


