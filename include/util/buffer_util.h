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
#include "Builder.h"


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

class FrameBuffer: public Builder{
private:
    std::vector<Texture2D> colors;
    GLuint object        = 0;
    GLuint depth_stencil = 0;
    bool depth      = false;
    bool stencil    = false;
    bool useRBO     = false;
    friend void swapTexture(FrameBuffer* f1, FrameBuffer* f2, int i1, int i2);

public:
    const GLsizei width;
    const GLsizei height;
    FrameBuffer(GLsizei width, GLsizei height);
    ~FrameBuffer();

    FrameBuffer& texture(GLint internalFormat, GLsizei n = 1, GLint warp = GL_CLAMP_TO_EDGE, GLint filter = GL_LINEAR);
    FrameBuffer& withDepth();
    FrameBuffer& withStencil();
    FrameBuffer& useRenderBuffer();
    void build() override;

    void bind() const;
    void unbind() const;
    void blitDepth(const FrameBuffer& input, GLenum bits) const;
    [[nodiscard]] Texture2D getDepthStencilTex() const;
    [[nodiscard]] Texture2D getTexture(int i = 0) const;
    [[nodiscard]] Texture2D extractTexture(int i = 0);
};


class FrameBufferCube: Builder {
private:
    TextureCube* color  = nullptr;
    GLuint object       = 0;
    GLuint depthCube    = 0;
    bool built = false;
    bool depth = false;

public:
    const GLsizei length;
    explicit FrameBufferCube(GLsizei length);
    FrameBufferCube& texture(GLint internalFormat, GLint warp = GL_CLAMP_TO_EDGE, GLint filter = GL_LINEAR);
    FrameBufferCube& withDepth();
    void build() override;

    [[nodiscard]] TextureCube getDepthStencilTex() const;
    [[nodiscard]] TextureCube getTexture() const;

    void bind() const;
    void bind(GLenum target) const;
    [[deprecated]]FrameBufferCube& withStencil();
};


#endif //ISLAND_BUFFER_UTIL_H
