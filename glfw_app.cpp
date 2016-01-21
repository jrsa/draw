#include <glog/logging.h>
#include "glfw_app.hpp"

glfw_app::glfw_app(std::function<void()> draw) : draw_proc(draw) {
  if (!glfwInit()) {
    LOG(FATAL) << "failed to initialize glfw";
  }

  glfwDefaultWindowHints();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _window = glfwCreateWindow(640, 480, "glfw_app", nullptr, nullptr);

  if (!_window) {
    LOG(FATAL) << "failed to create window";
  }
}

glfw_app::glfw_app()
    : glfw_app::glfw_app([] { LOG(ERROR) << "no draw function set"; }) {}

glfw_app::~glfw_app() { glfwTerminate(); }

void glfw_app::run() {

  glfwMakeContextCurrent(_window);
  while (!glfwWindowShouldClose(_window)) {
    draw_proc();
    glfwSwapBuffers(_window);
    glfwPollEvents();
  }
}
