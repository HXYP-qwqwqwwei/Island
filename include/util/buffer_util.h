//
// Created by HXYP on 2023/4/26.
//

#ifndef ISLAND_BUFFER_UTIL_H
#define ISLAND_BUFFER_UTIL_H
#include <iostream>
#include "glad/glad.h"

#define COLOR 0x1
#define DEPTH 0x2
#define STENCIL 0x4

class FrameBuffer {
public:
    FrameBuffer(GLsizei width, GLsizei height, int mode, bool readable = false);
    void bind() const;
    [[nodiscard]] GLuint getTexture() const;
private:
    GLuint fbo{};
    GLuint rbo{};
    GLuint color{};
};

#endif //ISLAND_BUFFER_UTIL_H
