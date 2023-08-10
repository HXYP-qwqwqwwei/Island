//
// Created by HXYP on 2023/4/26.
//

#ifndef ISLAND_BUFFER_UTIL_H
#define ISLAND_BUFFER_UTIL_H
#include <iostream>
#include <stack>
#include "glad/glad.h"
#include "defs.h"
#include "texture_util.h"


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

class FrameBuffer;

void swapTexture(FrameBuffer* f1, FrameBuffer* f2, int i1 = 0, int i2 = 0);

class FrameBuffer {
private:
    std::vector<Texture2D> colors;
    GLuint object        = 0;
    GLuint depth_stencil = 0;
    bool built      = false;
    bool depth      = false;
    bool stencil    = false;
    bool useRBO     = false;
    friend void swapTexture(FrameBuffer* f1, FrameBuffer* f2, int i1, int i2);

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
    void blitDepth(const FrameBuffer& input, GLenum bits) const;
    [[nodiscard]] bool checkBuilt() const;
    [[nodiscard]] TextureCube getDepthStencilTex() const;
    [[nodiscard]] Texture2D getTexture(int i = 0) const;
};


class FrameBufferCube {
public:
    FrameBufferCube(GLsizei length, GLint colorFormat, bool depth);
    [[nodiscard]] TextureCube getDepthCubeMap() const;
    [[nodiscard]] TextureCube getTextureCubeTex() const;
    void bind() const;
    const GLsizei length;
    const GLint colorFormat;
private:
    GLuint object{};
    GLuint colorCube{};
    GLuint depthCube{};
};


#endif //ISLAND_BUFFER_UTIL_H
