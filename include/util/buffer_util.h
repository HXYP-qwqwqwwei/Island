//
// Created by HXYP on 2023/4/26.
//

#ifndef ISLAND_BUFFER_UTIL_H
#define ISLAND_BUFFER_UTIL_H
#include <iostream>
#include <stack>
#include "glad/glad.h"
#include "defs.h"

#define COLOR 0x1       // 0
#define DEPTH 0x2       // 1
#define STENCIL 0x4     // 2
#define MSAA(S) ((S - 1) << 3)  // 3 ~ 6
#define MSAA_SAMPLES(M) (((M >> 3) & 0xF) + 1)
#define HDR 0x80        // 7

class Buffer {
public:
    explicit Buffer(GLenum target);
    void putData(size_t sz, const void* data, GLenum usage = GL_STATIC_DRAW);
    void subData(GLintptr offset, GLsizeiptr sz, const void* data) const;
    void bind() const;
    void unbind() const;
    void bindBufferBase(uint index) const;
    [[nodiscard]] size_t getSize() const;
    ~Buffer();

protected:
    GLuint object{};
    size_t size{};
    const GLenum target;
};


class FrameBuffer {
public:
    FrameBuffer(GLsizei width, GLsizei height, int mode, bool readable = false);
    ~FrameBuffer();
    void bind() const;
    void unbind() const;
    void enableMSAA(int mode, int samples);
    [[nodiscard]] GLuint getDepthStencilTex() const;
    [[nodiscard]] GLuint getTexture() const;
private:
    const int mode;
    GLsizei width;
    GLsizei height;
    FrameBuffer* msBuffer = nullptr;    // multisampling buffer

    GLuint object{};
    GLuint rbo{};
    GLuint color{};
    GLuint depth_stencil{};
    const bool readable;
};

class FrameBufferCube {
public:
    FrameBufferCube(GLsizei length, int mode);
    [[nodiscard]] GLuint getDepthCubeMap() const;
    [[nodiscard]] GLuint getTextureCubeTex() const;
    void bind() const;

private:
    GLuint object{};
    GLuint colorCube{};
    GLuint depthCube{};
};


class FrameBufferFloat {
public:
private:
    GLuint object{};
};


#endif //ISLAND_BUFFER_UTIL_H
