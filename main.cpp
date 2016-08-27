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
fbo* filt = nullptr; fbo* filt2 = nullptr;

void keycb(GLFWwindow *window, int key, int, int, int) {
  switch (key) {
  case 'R': {
    LOG(INFO) << "reloading shader(s)";
    source = new shader(SRC_FN);
    dest = new shader(DEST_FN);
    break;
  }
    case 'S': {
      filt2->bind();
      source->use();
      bb->draw();
    }
  default: { break; }
  }
}

int main(int argc, char **argv) {

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    // TODO: really fucking need another way to not use pointers,
    // preferably explicit initialization of gl resources, or scoping
    // the billboard in such a way that RAII can work to allocate them
    // when the context is ready
    bb = new billboard();
    source = new shader(SRC_FN);
    dest = new shader(DEST_FN);
    filt = new fbo(1280, 960);
    filt2 = new fbo(1280, 960);

    // seed
    filt2->bind();
    source->use();
    bb->draw();

    dest->u1f("amp", 9.0);
    dest->u1f("width", 3.0);
    dest->u1f("scaleCoef", 0.1);

    dest->u2f("dims", glm::vec2(1280, 960));
  };

  auto draw_proc = [&] {
//    gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    dest->use();

    filt2->bind_tex();
    filt->bind();
    bb->draw();

    filt->bind_tex();
    filt2->bind();
    bb->draw();

    fbo::unbind_all();
    bb->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);

  gltest.set_key_proc(keycb);

  gltest.run();
  return 0;
}
