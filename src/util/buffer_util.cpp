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


static const GLenum ColorAttachments[8]{
    GL_COLOR_ATTACHMENT0,
    GL_COLOR_ATTACHMENT1,
    GL_COLOR_ATTACHMENT2,
    GL_COLOR_ATTACHMENT3,
    GL_COLOR_ATTACHMENT4,
    GL_COLOR_ATTACHMENT5,
    GL_COLOR_ATTACHMENT6,
    GL_COLOR_ATTACHMENT7,
};


//FrameBuffer::FrameBuffer(GLsizei width, GLsizei height, int mode, bool depthStencilReadable): width(width), height(height), mode(mode), depthStencilReadable(depthStencilReadable) {
//    bool colorMode = mode & (RGB_BYTE | RGB_FLOAT);
//    bool depthMode = mode & DEPTH;
//    bool stencilMode = mode & STENCIL;
//    bool hdr_enabled = mode & RGB_FLOAT;
//    int samples = MSAA_SAMPLES(mode);
//    glGenFramebuffers(1, &this->object);
//    glBindFramebuffer(GL_FRAMEBUFFER, this->object); // or GL_READ_FRAMEBUFFER / GL_DRAW_FRAMEBUFFER
//
//    /*================ colors ================*/
//    if (colorMode) {
//        int n = MRT_TARGETS(mode);
//        auto* textures = new GLuint[n];
//
//        GLenum target;
//        GLenum type;
//        GLint format;
//        if (hdr_enabled) {
//            type    = GL_FLOAT;
//            format  = GL_RGB16F;
//        } else {
//            type    = GL_UNSIGNED_BYTE;
//            format  = GL_RGB;
//        }
//
//        glGenTextures(n, textures);
//        for (int i = 0; i < n; ++i) {
//
//            if (samples > 1) {  // MSAA
//                target = GL_TEXTURE_2D_MULTISAMPLE;
//                glBindTexture(target, textures[i]);
//                // 最后一个参数表示MSAA过程中所有像素的子采样点的位置和个数都相同
//                glTexImage2DMultisample(target, samples, format, width, height, GL_TRUE);
//
//            } else {
//                target = GL_TEXTURE_2D;
//                glBindTexture(target, textures[i]);
//                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, type, nullptr);
//            }
//            glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//            glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//            glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//            glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            glBindTexture(target, 0);
//
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, target, textures[i], 0);
//        }
//        glDrawBuffers(n, ColorAttachments);
//
//        this->colors = textures;
//    }
//    else {
//        glDrawBuffer(GL_NONE);
//        glReadBuffer(GL_NONE);
//    }
//
//    /*================ depth/stencil ================*/
//    if (depthMode || stencilMode) {
//        GLint internalFormat, format, type, attachment;
//        if (stencilMode) {
//            internalFormat  = GL_DEPTH24_STENCIL8;
//            format          = GL_DEPTH_STENCIL;
//            attachment      = GL_DEPTH_STENCIL_ATTACHMENT;
//            type            = GL_UNSIGNED_INT_24_8;
//        } else {
//            internalFormat  = GL_DEPTH_COMPONENT;
//            format          = GL_DEPTH_COMPONENT;
//            attachment      = GL_DEPTH_ATTACHMENT;
//            type            = GL_FLOAT;
//        }
//        if (depthStencilReadable) {
//            // depth/stencil
//            glGenTextures(1, &this->depth_stencil);
//            glBindTexture(GL_TEXTURE_2D, this->depth_stencil);
//            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, this->depth_stencil, 0);
//
//        } else {    // 不需要读取缓冲，直接使用RBO
//            glGenRenderbuffers(1, &this->rbo);
//            glBindRenderbuffer(GL_RENDERBUFFER, this->rbo);
//            if (samples > 1) {
//                glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, width, height);
//            } else {
//                glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
//            }
//            glBindRenderbuffer(GL_RENDERBUFFER, 0);
//            glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, this->rbo);
//        }
//
//    }
//
//
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
//        std::cerr << "ERROR::FRAMEBUFFER::Incomplete framebuffer\n";
//    }
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}
//
//void FrameBuffer::bind() const {
//    if (this->msBuffer != nullptr) {
//        glBindFramebuffer(GL_FRAMEBUFFER, this->msBuffer->object);
//    }
//    else glBindFramebuffer(GL_FRAMEBUFFER, this->object);
//}
//
//GLuint FrameBuffer::getTexture(int i) const {
//    if (this->colors == nullptr) {
//        std::cerr << "WARN::FRAMEBUFFER::Framebuffer " << this->object << " not contains color buffer\n";
//        return 0;
//    }
//    return this->colors[i];
//}
//
//FrameBuffer::~FrameBuffer() {
//    glDeleteFramebuffers(1, &this->object);
//    glDeleteRenderbuffers(1, &this->rbo);
//    glDeleteTextures(MRT_TARGETS(mode), this->colors);
//    delete[] this->colors;
//    glDeleteTextures(1, &this->depth_stencil);
//    delete msBuffer;
//}
//
//void FrameBuffer::unbind() const {
//    if (this->msBuffer != nullptr) {    // Enabled MSAA: blit data
//        GLuint in = this->msBuffer->object;
//        GLuint out = this->object;
//        int nTargets = MRT_TARGETS(mode);
//        for (int i = 0; i < nTargets; ++i) {
//            glNamedFramebufferReadBuffer(in, GL_COLOR_ATTACHMENT0 + i);
//            glNamedFramebufferDrawBuffer(out, GL_COLOR_ATTACHMENT0 + i);
//            glBlitNamedFramebuffer(in, out, 0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
//        }
//    }
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//}
//
//void FrameBuffer::enableMSAA(int _mode, int samples) {
//    this->msBuffer = new FrameBuffer(width, height, _mode | MSAA(samples));
//}
//
//GLuint FrameBuffer::getDepthStencilTex() const {
//    if (this->depthStencilReadable) {
//        return this->depth_stencil;
//    }
//    std::cerr << "ERROR::FRAMEBUFFER::Cannot read depth/stencil texture when enabled render buffer\n";
//    return 0;
//}

FrameBufferCube::FrameBufferCube(GLsizei length, GLsizei maxMipmapLevels): length(length), mipmapLevels(maxMipmapLevels), Builder("FrameBufferCube") { }

TextureCube FrameBufferCube::getDepthStencilTex() const {
    return {this->depthCube, length, length, GL_DEPTH_COMPONENT};
}

TextureCube FrameBufferCube::getTexture() const {
    if (this->color.id == 0) {
        std::cerr << "WARN::FrameBufferCube::getTexture: buffer has no color attachment.\n";
    }
    return this->color;
}

TextureCube FrameBufferCube::extractTexture() {
    TextureCube extracted = this->color;
    if (extracted.id == 0) {
        std::cerr << "WARN::FrameBufferCube::texture: Cube has no color attachment.\n";
        return extracted;
    }
    this->color = createTextureCube(extracted.internalFormat, extracted.length, extracted.warp, extracted.filter, mipmapLevels > 1);
    return extracted;
}


FrameBufferCube &FrameBufferCube::texture(GLint internalFormat, GLint warp, GLint filter) {
    if (checkBuilt("texture: Buffer is already built.")) return *this;
    if (this->color.id != 0) {
        std::cerr << "WARN::FrameBufferCube::texture: Cube buffer can only create one color attachment.\n";
        return *this;
    }
    this->color = createTextureCube(internalFormat, this->length, warp, filter, mipmapLevels > 1);
    return *this;
}

FrameBufferCube &FrameBufferCube::withDepth() {
    if (checkBuilt("withDepth: buffer is already built.")) return *this;
    this->depth = true;
    return *this;
}

FrameBufferCube &FrameBufferCube::withStencil() {
    if (checkBuilt("withStencil: buffer is already built.")) return *this;
    return *this;
}

FrameBufferCube& FrameBufferCube::useRenderBuffer() {
    if (checkBuilt("useRenderBuffer: buffer is already built.")) return *this;
    this->useRBO = true;
    return *this;
}


void FrameBufferCube::build() {
    if (checkBuilt("build: Buffer is already built.")) return;

    glGenFramebuffers(1, &this->object);
    glBindFramebuffer(GL_FRAMEBUFFER, this->object);

    if (depth) {
        if (useRBO) {
            glGenRenderbuffers(1, &this->depthCube);
            glBindRenderbuffer(GL_RENDERBUFFER, this->depthCube);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, length, length);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->depthCube);
        } else {
            glGenTextures(1, &this->depthCube);
            glBindTexture(GL_TEXTURE_CUBE_MAP, this->depthCube);
            for (int i = 0; i < 6; ++i) {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, length, length, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            }
            if (mipmapLevels > 1) {
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipmap_filter(GL_NEAREST));
            } else {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthCube, 0);
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->setBuilt();
}

void FrameBufferCube::bind(GLsizei mipLevel) const {
    auto mipLen  = GLsizei(this->length * std::pow(0.5, mipLevel));
    glViewport(0, 0, mipLen, mipLen);

    glBindFramebuffer(GL_FRAMEBUFFER, this->object);
    if (this->color.id != 0) {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->color.id, mipLevel);
    } else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (this->depth) {
        if (this->useRBO) {
            glBindRenderbuffer(GL_RENDERBUFFER, this->depthCube);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mipLen, mipLen);
        } else {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, this->depthCube, mipLevel);
        }
    }
}

void FrameBufferCube::bind(GLenum target, GLsizei mipLevel) const {
    auto mipLen  = GLsizei(this->length * std::pow(0.5, mipLevel));
    glViewport(0, 0, mipLen, mipLen);
    glBindFramebuffer(GL_FRAMEBUFFER, this->object);
    if (this->color.id != 0) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, this->color.id, mipLevel);
    } else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    if (this->depth) {
        if (this->useRBO) {
            glBindRenderbuffer(GL_RENDERBUFFER, this->depthCube);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mipLen, mipLen);
        } else {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, this->depthCube, mipLevel);
        }
    }
}


FrameBuffer::FrameBuffer(GLsizei width, GLsizei height): width(width), height(height), Builder("FrameBuffer") {}


FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &this->object);
    if (depth || stencil) {
        if (useRBO) glDeleteRenderbuffers(1, &this->depth_stencil);
        else glDeleteTextures(1, &this->depth_stencil);
    }
    for (auto& tex : this->colors) {
        glDeleteTextures(1, &tex.id);
    }
}


FrameBuffer& FrameBuffer::texture(GLint internalFormat, GLsizei n, GLint warp, GLint filter) {
    if (checkBuilt("texture: Buffer is already built.")) return *this;
    n = MAX(1, n);
    for (int i = 0; i < n; ++i) {
        GLuint tex;
        glGenTextures(1, &tex);

        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, warp);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, warp);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        glBindTexture(GL_TEXTURE_2D, 0);

        this->colors.emplace_back(tex, width, height, internalFormat, warp, filter);
    }
    return *this;
}

FrameBuffer& FrameBuffer::withDepth() {
    if (checkBuilt("withDepth: Buffer is already built.")) return *this;
    this->depth = true;
    return *this;
}

FrameBuffer& FrameBuffer::withStencil() {
    if (checkBuilt("withStencil: Buffer is already built.")) return *this;
    this->stencil = true;
    return *this;
}

FrameBuffer &FrameBuffer::useRenderBuffer() {
    if (checkBuilt("useRenderBuffer: Buffer is already built.")) return *this;
    this->useRBO = true;
    return *this;
}

void FrameBuffer::build() {
    if (checkBuilt("build: Buffer is already built.")) return;
    glGenFramebuffers(1, &this->object);
    glBindFramebuffer(GL_FRAMEBUFFER, this->object);

    GLsizei nTex = MIN(this->colors.size(), 8);
    if (nTex == 0) {
        glReadBuffer(GL_NONE);
        glDrawBuffer(GL_NONE);
    } else {
        for (int i = 0; i < nTex; ++i) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->colors[i].id, 0);
        }
        glDrawBuffers(nTex, ColorAttachments);
    }

    /*================ depth/stencil ================*/
    if (depth || stencil) {
        GLint internalFormat, format, type, attachment;
        if (stencil) {
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
        if (useRBO) {
            glGenRenderbuffers(1, &this->depth_stencil);
            glBindRenderbuffer(GL_RENDERBUFFER, this->depth_stencil);
            glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, this->depth_stencil);
        } else {
            glGenTextures(1, &this->depth_stencil);
            glBindTexture(GL_TEXTURE_2D, this->depth_stencil);
            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, this->depth_stencil, 0);
        }

    }


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER::Incomplete framebuffer." << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->setBuilt();
}


Texture2D FrameBuffer::getDepthStencilTex() const {
    if (this->checkNotBuilt("getDepthStencilTex: Buffer has not built.")) {
        return {};
    }
    if (this->useRBO) {
        std::cerr << "ERROR::FRAMEBUFFER::Cannot read depth/stencil buffer when enabled render buffer" << std::endl;
        return {};
    }
    return {this->depth_stencil, width, height, stencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT};
}


Texture2D FrameBuffer::getTexture(int i) const {
    if (this->checkNotBuilt("getTexture: Buffer has not built.")) {
        return {};
    }
    return this->colors[i];
}

Texture2D FrameBuffer::extractTexture(int i) {
    Texture2D extracted = this->colors[i];
    this->colors[i] = createTexture2D(extracted.internalFormat, extracted.width, extracted.height, extracted.warp, extracted.filter);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, this->colors[i].id, 0);
    return extracted;
}


void FrameBuffer::bind() const {
    this->checkNotBuilt("bind: Buffer has not built");
    glBindFramebuffer(GL_FRAMEBUFFER, this->object);
}

void FrameBuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::blitDepth(const FrameBuffer &input, GLenum bits) const {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, input.object);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->object);
    glBlitFramebuffer(0, 0, input.width, input.height, 0, 0, width, height, bits, GL_NEAREST);
}

void swapTexture(FrameBuffer *f1, FrameBuffer *f2, int i1, int i2) {
    Texture2D& tex1 = f1->colors[i1];
    Texture2D& tex2 = f2->colors[i2];
    if (tex1.internalFormat != tex2.internalFormat) {
        std::cerr << "ERROR::SWAP_TEXTURE::" << "Texture " << tex1.id << "and texture" << tex2.id << "has different internal format\n";
        return;
    }
    GLuint id1 = tex1.id;
    tex1.id = tex2.id;
    tex2.id = id1;

    f1->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i1, GL_TEXTURE_2D, tex1.id, 0);
    f2->bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i2, GL_TEXTURE_2D, tex2.id, 0);
}
