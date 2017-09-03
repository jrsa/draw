#include <glog/logging.h>

#include <glbinding/Binding.h>
#include <glbinding/Meta.h>
#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>

#include <gl_shared.hpp>
#include <iomanip>
#include <sstream>

#include <glfw_app.hpp>

glfw_app::glfw_app(std::function<void()> draw, std::function<void()> setup)
    : _draw_proc(draw), _setup_proc(setup) {
  if (!glfwInit()) {
    LOG(FATAL) << "failed to initialize glfw";
  }

  glfwDefaultWindowHints();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  _window = glfwCreateWindow(640, 480, "", nullptr, nullptr);

  if (!_window) {
    LOG(FATAL) << "failed to create window";
  }
  glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

glfw_app::~glfw_app() { glfwTerminate(); }

void glfw_app::run() {
  glfwMakeContextCurrent(_window);

  glbinding::Binding::initialize(false);
  glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, { "glGetError" });
  glbinding::setAfterCallback([](const glbinding::FunctionCall& call) {
    const auto error = gl::glGetError();
    if (error != gl::GL_NO_ERROR) {
      LOG(ERROR) << "error in " << call.function->name() << ": " << std::hex << error; 
    }
  });

  LOG(INFO) << "OpenGL " << glbinding::ContextInfo::version()  
            << ", " << glbinding::ContextInfo::renderer();

  _setup_proc();
  while (!glfwWindowShouldClose(_window)) {

    _draw_proc();
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

void glfw_app::set_cursor_proc(GLFWcursorposfun cbfun) {
  glfwSetCursorPosCallback(_window, cbfun);
}