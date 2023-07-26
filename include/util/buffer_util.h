//
// Created by HXYP on 2023/4/26.
//

#ifndef ISLAND_BUFFER_UTIL_H
#define ISLAND_BUFFER_UTIL_H
#include <iostream>
#include <stack>
#include "glad/glad.h"
#include "defs.h"

#define RGB_BYTE    0x0
#define RGB_FLOAT   0x1
#define RGBA_BYTE   0x2
#define RGBA_FLOAT  0x3
#define DEPTH       0x2                 // 1
#define STENCIL     0x4                 // 2
//#define MSAA(S)     (((S-1) & 0xF)<<3)  // 3 ~ 6
//#define MRT(N)      (((N-1) & 0x7)<<8)  // 8 ~ 10

#define MSAA_SAMPLES(M) (((M >> 3) & 0xF) + 1)
#define MRT_TARGETS(M)  (((M >> 8) & 0x7) + 1)

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


//class FrameBuffer {
//public:
//    FrameBuffer(GLsizei width, GLsizei height, int mode, bool depthStencilReadable = false);
//    ~FrameBuffer();
//    void bind() const;
//    void unbind() const;
//    void enableMSAA(int mode, int samples);
//    [[nodiscard]] GLuint getDepthStencilTex() const;
//    [[nodiscard]] GLuint getTexture(int i = 0) const;
//    const GLsizei width;
//    const GLsizei height;
//private:
//    const int mode;
//    FrameBuffer* msBuffer = nullptr;    // multisampling buffer
//
//    GLuint object{};
//    GLuint rbo{};
//    GLuint* colors = nullptr;
//    GLuint depth_stencil{};
//    const bool depthStencilReadable;
//};

class FrameBuffer {
private:
    std::vector<GLuint> colors;
    GLuint object        = 0;
    GLuint depth_stencil = 0;
    bool built      = false;
    bool depth      = false;
    bool stencil    = false;
    bool useRBO     = false;

public:
    const GLsizei width;
    const GLsizei height;
    FrameBuffer(GLsizei width, GLsizei height);
    ~FrameBuffer();

    FrameBuffer& texture(int mode, int n = 1, GLint warp = GL_CLAMP_TO_EDGE, GLint filter = GL_LINEAR);
    FrameBuffer& depthBuffer();
    FrameBuffer& stencilBuffer();
    FrameBuffer& useRenderBuffer();
    void build();

    void bind() const;
    void unbind() const;
    [[nodiscard]] bool checkBuilt() const;
    [[nodiscard]] GLuint getDepthStencilTex() const;
    [[nodiscard]] GLuint getTexture(int i = 0) const;
};



class FrameBufferCube {
public:
    FrameBufferCube(GLsizei length, int mode);
    [[nodiscard]] GLuint getDepthCubeMap() const;
    [[nodiscard]] GLuint getTextureCubeTex() const;
    void bind() const;
    const GLsizei length;
private:
    GLuint object{};
    GLuint colorCube{};
    GLuint depthCube{};
};


struct TextureBuffer {
    GLuint id = 0;
    TextureBuffer(GLsizei width, GLsizei height, int mode, GLint warp = GL_CLAMP_TO_EDGE, GLint filter = GL_LINEAR);
    void free();
};


class GBuffer {

};

#endif //ISLAND_BUFFER_UTIL_H
