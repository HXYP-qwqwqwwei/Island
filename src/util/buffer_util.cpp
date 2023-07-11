//
// Created by HXYP on 2023/4/26.
//

#include "util/buffer_util.h"

Buffer::Buffer(GLenum target): target(target) {
    glGenBuffers(1, &this->object);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &this->object);
}

void Buffer::putData(size_t sz, const void *data, GLenum usage) {
    this->bind();
    glBufferData(this->target, static_cast<Long>(sz), data, usage);
    this->size = sz;
    this->unbind();
}

void Buffer::subData(GLintptr offset, GLsizeiptr sz, const void *data) const {
    this->bind();
    glBufferSubData(this->target, offset, sz, data);
    this->unbind();
}

void Buffer::bind() const {
    glBindBuffer(this->target, this->object);
}

void Buffer::unbind() const {
    glBindBuffer(this->target, 0);
}

void Buffer::bindBufferBase(uint index) const {
    switch (this->target) {
        case GL_ATOMIC_COUNTER_BUFFER:
        case GL_TRANSFORM_FEEDBACK_BUFFER:
        case GL_UNIFORM_BUFFER:
        case GL_SHADER_STORAGE_BUFFER:
            glBindBufferBase(this->target, index, this->object);
            return;
    }
    std::cout << "WARN::BUFFER::Target 0x"
            << std::hex << std::uppercase << this->target
            << std::nouppercase << " is not an indexed buffer target.\n";
}

size_t Buffer::getSize() const {
    return this->size;
}


FrameBuffer::FrameBuffer(GLsizei width, GLsizei height, int mode, bool readable): width(width), height(height), mode(mode), readable(readable) {
    bool colorMode = mode & COLOR;
    bool depthMode = mode & DEPTH;
    bool stencilMode = mode & STENCIL;
    int samples = MSAA_SAMPLES(mode);
    glGenFramebuffers(1, &this->object);
    glBindFramebuffer(GL_FRAMEBUFFER, this->object); // or GL_READ_FRAMEBUFFER / GL_DRAW_FRAMEBUFFER

    /*================ color ================*/
    if (colorMode) {
        glGenTextures(1, &this->color);
        GLenum target;
        if (samples > 1) {  // MSAA
            target = GL_TEXTURE_2D_MULTISAMPLE;
            glBindTexture(target, this->color);
            // 最后一个参数表示MSAA过程中所有像素的子采样点的位置和个数都相同
            glTexImage2DMultisample(target, samples, GL_SRGB, width, height, GL_TRUE);
        } else {
            target = GL_TEXTURE_2D;
            glBindTexture(target, this->color);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        }
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(target, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, this->color, 0);
    }
    else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    /*================ depth/stencil ================*/
    if (depthMode || stencilMode) {
        GLint internalFormat, format, type, attachment;
        if (stencilMode) {
            internalFormat = GL_DEPTH24_STENCIL8;
            format = GL_DEPTH_STENCIL;
            attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            type = GL_UNSIGNED_INT_24_8;
        } else {
            internalFormat = GL_DEPTH_COMPONENT;
            format = GL_DEPTH_COMPONENT;
            attachment = GL_DEPTH_ATTACHMENT;
            type = GL_FLOAT;
        }
        if (readable) {
            // depth/stencil
            glGenTextures(1, &this->depth_stencil);
            glBindTexture(GL_TEXTURE_2D, this->depth_stencil);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, this->depth_stencil, 0);

        } else {    // 不需要读取缓冲，直接使用RBO
            glGenRenderbuffers(1, &this->rbo);
            glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
            if (samples > 1) {
                glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, width, height);
            } else {
                glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
            }
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, this->rbo);
        }

    }


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER::Incomplete framebuffer\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bind() const {
    if (this->msBuffer != nullptr) {
        glBindFramebuffer(GL_FRAMEBUFFER, this->msBuffer->object);
    }
    else glBindFramebuffer(GL_FRAMEBUFFER, this->object);
}

GLuint FrameBuffer::getTexture() const {
    return this->color;
}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &this->object);
    glDeleteRenderbuffers(1, &this->rbo);
    glDeleteTextures(1, &this->color);
    glDeleteTextures(1, &this->depth_stencil);
    delete msBuffer;
}

void FrameBuffer::unbind() const {
    if (this->msBuffer != nullptr) {    // Enabled MSAA: blit data
        glBindFramebuffer(GL_READ_FRAMEBUFFER, this->msBuffer->object);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->object);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::enableMSAA(int _mode, int samples) {
    this->msBuffer = new FrameBuffer(width, height, _mode | MSAA(samples));
}

GLuint FrameBuffer::getDepthStencilTex() const {
    if (this->readable) {
        return this->depth_stencil;
    }
    std::cerr << "ERROR::FRAMEBUFFER::Cannot read depth/stencil texture when enabled render buffer\n";
    return 0;
}

FrameBufferCube::FrameBufferCube(GLsizei length, int mode) {
    glGenFramebuffers(1, &this->object);
    glBindFramebuffer(GL_FRAMEBUFFER, this->object);
    if (mode & COLOR) {
        glGenTextures(1, &this->colorCube);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->colorCube);
        for (int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB, length, length, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->colorCube, 0);

    } else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (mode & DEPTH) {
        glGenTextures(1, &this->depthCube);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthCube);
        for (int i = 0; i < 6; ++i) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, length, length, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthCube, 0);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FrameBufferCube::getDepthCubeMap() const {
    return this->depthCube;
}

GLuint FrameBufferCube::getTextureCubeTex() const {
    return this->colorCube;
}

void FrameBufferCube::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, this->object);
}