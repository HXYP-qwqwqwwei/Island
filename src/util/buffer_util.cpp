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


FrameBuffer::FrameBuffer(GLsizei width, GLsizei height, int mode, bool depthStencilReadable): width(width), height(height), mode(mode), depthStencilReadable(depthStencilReadable) {
    bool colorMode = mode & (COLOR_LDR | COLOR_HDR);
    bool depthMode = mode & DEPTH;
    bool stencilMode = mode & STENCIL;
    bool hdr_enabled = mode & COLOR_HDR;
    int samples = MSAA_SAMPLES(mode);
    glGenFramebuffers(1, &this->object);
    glBindFramebuffer(GL_FRAMEBUFFER, this->object); // or GL_READ_FRAMEBUFFER / GL_DRAW_FRAMEBUFFER

    /*================ colors ================*/
    if (colorMode) {
        int n = MRT_TARGETS(mode);
        auto* textures = new GLuint[n];

        GLenum target;
        GLenum type;
        GLint format;
        if (hdr_enabled) {
            type    = GL_FLOAT;
            format  = GL_RGB16F;
        } else {
            type    = GL_UNSIGNED_BYTE;
            format  = GL_RGB;
        }

        glGenTextures(n, textures);
        for (int i = 0; i < n; ++i) {

            if (samples > 1) {  // MSAA
                target = GL_TEXTURE_2D_MULTISAMPLE;
                glBindTexture(target, textures[i]);
                // 最后一个参数表示MSAA过程中所有像素的子采样点的位置和个数都相同
                glTexImage2DMultisample(target, samples, format, width, height, GL_TRUE);

            } else {
                target = GL_TEXTURE_2D;
                glBindTexture(target, textures[i]);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, type, nullptr);
            }
            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(target, 0);

            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, target, textures[i], 0);
        }
        static const GLenum attachments[2]{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(n, attachments);

        this->colors = textures;
    }
    else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    /*================ depth/stencil ================*/
    if (depthMode || stencilMode) {
        GLint internalFormat, format, type, attachment;
        if (stencilMode) {
            internalFormat  = GL_DEPTH24_STENCIL8;
            format          = GL_DEPTH_STENCIL;
            attachment      = GL_DEPTH_STENCIL_ATTACHMENT;
            type            = GL_UNSIGNED_INT_24_8;
        } else {
            internalFormat  = GL_DEPTH_COMPONENT;
            format          = GL_DEPTH_COMPONENT;
            attachment      = GL_DEPTH_ATTACHMENT;
            type            = GL_FLOAT;
        }
        if (depthStencilReadable) {
            // depth/stencil
            glGenTextures(1, &this->depth_stencil);
            glBindTexture(GL_TEXTURE_2D, this->depth_stencil);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, this->depth_stencil, 0);

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

GLuint FrameBuffer::getTexture(int i) const {
    if (this->colors == nullptr) {
        std::cerr << "WARN::FRAMEBUFFER::Framebuffer " << this->object << " not contains color buffer\n";
        return 0;
    }
    return this->colors[i];
}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &this->object);
    glDeleteRenderbuffers(1, &this->rbo);
    glDeleteTextures(MRT_TARGETS(mode), this->colors);
    delete[] this->colors;
    glDeleteTextures(1, &this->depth_stencil);
    delete msBuffer;
}

void FrameBuffer::unbind() const {
    if (this->msBuffer != nullptr) {    // Enabled MSAA: blit data
        GLuint in = this->msBuffer->object;
        GLuint out = this->object;
        int nTargets = MRT_TARGETS(mode);
        for (int i = 0; i < nTargets; ++i) {
            glNamedFramebufferReadBuffer(in, GL_COLOR_ATTACHMENT0 + i);
            glNamedFramebufferDrawBuffer(out, GL_COLOR_ATTACHMENT0 + i);
            glBlitNamedFramebuffer(in, out, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::enableMSAA(int _mode, int samples) {
    this->msBuffer = new FrameBuffer(width, height, _mode | MSAA(samples));
}

GLuint FrameBuffer::getDepthStencilTex() const {
    if (this->depthStencilReadable) {
        return this->depth_stencil;
    }
    std::cerr << "ERROR::FRAMEBUFFER::Cannot read depth/stencil texture when enabled render buffer\n";
    return 0;
}

FrameBufferCube::FrameBufferCube(GLsizei length, int mode): length(length) {
    glGenFramebuffers(1, &this->object);
    glBindFramebuffer(GL_FRAMEBUFFER, this->object);
    if (mode & COLOR_LDR) {
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