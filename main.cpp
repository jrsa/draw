#include <glbinding/Binding.h>
#include <glog/logging.h>

#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "model.hpp"
#include "shader.hpp"

#define SHADER_FN "/Users/jrsa/code/gl/draw/glsl/2"

model* m = nullptr;
shader* s = nullptr;

void keycb(GLFWwindow *window, int key, int, int, int) {
  switch (key) {
  case 'R': {
    LOG(INFO) << "reloading shader";
    s = new shader(SHADER_FN);
    break;
  }
  default: { break; }
  }
}

int main(int argc, char **argv) {

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    // TODO: really fucking need another way to not use pointers,
    // preferably explicit initialization of gl resources, or scoping
    // the model in such a way that RAII can work to allocate them
    // when the context is ready
    m = new model();
    s = new shader(SHADER_FN);
  };

  auto draw_proc = [&] {
    gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    s->use();
    m->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);

  gltest.set_key_proc(keycb);

  gltest.run();
  return 0;
}
