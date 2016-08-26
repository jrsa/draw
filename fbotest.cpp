#include <glbinding/Binding.h>
#include <glog/logging.h>

#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "billboard.hpp"
#include "shader.hpp"
#include "fbo.h"

#define SRC_FN "/Users/jrsa/code/gl/draw/glsl/3"
#define DEST_FN "/Users/jrsa/code/gl/draw/glsl/dest"

billboard* bb = nullptr;
shader* source = nullptr;
shader* dest = nullptr;
fbo* filt = nullptr;

int main(int argc, char **argv) {

  GLuint fbo = 0, tex = 0;

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    bb = new billboard();
    source = new shader(SRC_FN);
    dest = new shader(DEST_FN);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, (int)GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int)GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int)GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  };

  auto draw_proc = [&] {
    gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    source->use();
    bb->draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    dest->use();
    bb->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);

  gltest.run();
  return 0;
}
