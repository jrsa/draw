#include "fbo.h"
#include "gl_shared.hpp"

using namespace gl;

fbo::fbo(int h, int w): _colorbuffer_texture_obj(0), _fbo_obj(0) {
    glGenFramebuffers(1, &_fbo_obj);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo_obj);

    glGenTextures(1, &_colorbuffer_texture_obj);

    // upload an empty texture, not confirmed necessary
    glBindTexture(GL_TEXTURE_2D, _colorbuffer_texture_obj);
    glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glTextureParameteri((int)GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
    glTextureParameteri((int)GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorbuffer_texture_obj, 0);
}

fbo::~fbo() {
    glDeleteFramebuffers(1, &_fbo_obj);
}

void fbo::bind() {
    glBindFramebuffer(gl::GL_FRAMEBUFFER, _fbo_obj);
}

void fbo::bind_tex() {
    glBindTexture(GL_TEXTURE_2D, _colorbuffer_texture_obj);
}

bool fbo::is_complete() {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void fbo::unbind_all() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}