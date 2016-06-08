#include <glbinding/Binding.h>
#include <glog/logging.h>

#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "model.hpp"

int main(int argc, char **argv) {

  model* m = nullptr;

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    // TODO: really fucking need another way to not use pointers,
    // preferably explicit initialization of gl resources, or scoping
    // the model in such a way that RAII can work to allocate them
    // when the context is ready
    m = new model();
  };

  auto draw_proc = [&] {
    gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);

  gltest.run();
  return 0;
}
