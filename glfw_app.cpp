#include <glog/logging.h>
#include "glfw_app.hpp"

glfw_app::glfw_app(std::function<void()> draw, std::function<void()> setup)
    : draw_proc(draw), setup_proc(setup) {
  if (!glfwInit()) {
    LOG(FATAL) << "failed to initialize glfw";
  }

  glfwDefaultWindowHints();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _window = glfwCreateWindow(640, 480, "", nullptr, nullptr);

  if (!_window) {
    LOG(FATAL) << "failed to create window";
  }
}

glfw_app::~glfw_app() { glfwTerminate(); }

void glfw_app::run() {

  glfwMakeContextCurrent(_window);
  setup_proc();
  while (!glfwWindowShouldClose(_window)) {
    draw_proc();
    glfwSwapBuffers(_window);
    glfwPollEvents();
  }
}

void glfw_app::set_key_proc(GLFWkeyfun _kp) {
   glfwSetKeyCallback(_window, _kp);
}

void glfw_app::set_fbsize_proc(GLFWframebuffersizefun _p) {
  glfwSetFramebufferSizeCallback(_window, _p);
}