//
// Created by HXYP on 2023/4/12.
//

#include "util/shapes.h"

Model shapes::Cube(float len, TexList<Texture2DWithType> textures) {
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

Model shapes::Rectangle(float w, float h, TexList<Texture2DWithType> textures, float maxU, float maxV) {
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

BuiltinMesh* shapes::BallMesh(GLfloat radius, GLsizei segmentsXZ, GLsizei segmentsY, GLfloat maxU, GLfloat maxV) {
    std::vector<glm::vec3> positions;

    // vertex positions
    float dAlpha = AI_MATH_PI / segmentsY;
    float dBeta = AI_MATH_TWO_PI / segmentsXZ;
    glm::vec3 top(0, +radius, 0);
    glm::vec3 bot(0, -radius, 0);
    for (int i = 1; i < segmentsY; ++i) {
        float alpha = dAlpha * (float)i;
        float y = radius * glm::cos(alpha);
        for (int j = 0; j < segmentsXZ; ++j) {
            float beta = -dBeta * (float)j;
            float r = radius * glm::sin(alpha);
            float x = r * glm::cos(beta);
            float z = r * glm::sin(beta);
            positions.emplace_back(x, y, z);
        }
    }

    BuiltinMesh& ball = *(new BuiltinMesh);
    const GLfloat dU = maxU / GLfloat(segmentsXZ);
    const GLfloat dV = maxV / GLfloat(segmentsY);

    auto calUV = [=](GLsizei xz, GLsizei y) -> glm::vec2 {
        return {GLfloat(xz) * dU, 1.0f - GLfloat(y) * dV};
    };

    // top
    for (int i = 0; i < segmentsXZ; ++i) {
        ball.addVertex(top, calUV(i, 0))
            .addVertex(positions[i], calUV(i, 1))
            .addVertex(positions[(i+1) % segmentsXZ], calUV((i+1) % segmentsXZ, 1))
            .nextFace();
    }

    for (int i = 0; i < segmentsY-2; ++i) {
        int first1 = i * segmentsXZ;
        int first2 = first1 + segmentsXZ;
        for (int j = 0; j < segmentsXZ; ++j) {
            int v1 = first1 + j, v2 = (j+1) % segmentsXZ + first1;
            int w1 = first2 + j, w2 = (j+1) % segmentsXZ + first2;
            ball.addVertex(positions[v1], calUV(j, i))      /*  v1   v2 */
                .addVertex(positions[w1], calUV(j, i+1))    /*  |  \    */
                .addVertex(positions[w2], calUV(j+1, i+1))  /*  w1—— w2 */
                .nextFace()
                .addVertex(positions[v1], calUV(j, i))      /*  v1 ——v2 */
                .addVertex(positions[w2], calUV(j+1, i+1))  /*     \  | */
                .addVertex(positions[v2], calUV(j+1, i))    /*  w1   w2 */
                .nextFace();
        }
    }

    // bottom
    int first = (segmentsY-2) * segmentsXZ;
    for (int i = 0; i < segmentsXZ; ++i) {
        ball.addVertex(positions[first + i], calUV(i, segmentsXZ - 1))
            .addVertex(bot, calUV(i, segmentsXZ))
            .addVertex(positions[first + (i+1) % segmentsXZ], calUV((i+1) % segmentsXZ, segmentsXZ - 1))
            .nextFace();
    }
    return &ball;
}

Model shapes::Ball(GLfloat radius, GLsizei segmentsXZ, GLsizei segmentsY, TexList<Texture2DWithType> textures, GLfloat maxU, GLfloat maxV) {
    auto mesh = BallMesh(radius, segmentsXZ, segmentsY, maxU, maxV);
    mesh->setTextures(textures);
    Model ball({mesh->build()});
    delete mesh;
    return ball;
}



Screen* shapes::ScreenRect(TexList<Texture2D> tex) {
    std::vector<Texture2D> textures(tex);
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

SkyBox* shapes::SkyBoxCube(TextureCube texture) {
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

    std::vector<TextureCube> textures = {texture};
    return new SkyBox(vertices, indices, textures);
}


