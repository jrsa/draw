#include <glbinding/Binding.h>
#include <glog/logging.h>

#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "model.hpp"

int main(int argc, char **argv) {

  glbinding::Binding::initialize(false);

  glfw_app gltest;
  model m; // drawable/shadable

  gltest.draw_proc = [&] {
    gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m.draw();
  };

  gltest.run();
  return 0;
}
