//
// Created by HXYP on 2023/4/26.
//

#include "util/buffer_util.h"

FrameBuffer::FrameBuffer(GLsizei width, GLsizei height, int mode, bool readable) {
    bool colorMode = mode & COLOR;
    bool depthMode = mode & DEPTH;
    bool stencilMode = mode & STENCIL;
    glGenFramebuffers(1, &this->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo); // or GL_READ_FRAMEBUFFER / GL_DRAW_FRAMEBUFFER
    // color
    if (colorMode) {
        glGenTextures(1, &this->color);
        glBindTexture(GL_TEXTURE_2D, this->color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->color, 0);
    } else {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    // depth/stencil
    if (depthMode || stencilMode) {
        // depth/stencil
//        glBindTexture(GL_TEXTURE_2D, fboTex[1]);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, gameScrWidth, gameScrHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fboTex[1], 0);
        GLenum format, attachment;
        // TODO
        if (depthMode && stencilMode) {
            format = GL_DEPTH24_STENCIL8;
            attachment = GL_DEPTH_STENCIL_ATTACHMENT;
        } else if (depthMode) {
            format = GL_DEPTH_COMPONENT;
            attachment = GL_DEPTH_ATTACHMENT;
        } else {
            format = GL_STENCIL_COMPONENTS;
        }

        GLuint RBO;
        glGenRenderbuffers(1, &RBO);
        glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

    }
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "ERROR::FRAMEBUFFER::Incomplete framebuffer\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, this->fbo);
}

GLuint FrameBuffer::getTexture() const {
    return this->color;
}