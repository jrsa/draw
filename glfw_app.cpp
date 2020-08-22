#include <glog/logging.h>

#include <glbinding/Binding.h>
#include <glbinding/Meta.h>
#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>

#include <gl_shared.hpp>
#include <iomanip>
#include <sstream>

#include <glfw_app.hpp>

std::map<GLFWwindow*, glfw_app*> glfw_app::s_instances_;

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


  // install "static" callbacks
  glfwSetFramebufferSizeCallback(_window, glfw_app::s_fbsizeCb);
  glfwSetKeyCallback(_window, glfw_app::s_keyCb);
  glfwSetCursorPosCallback(_window, glfw_app::s_mouseCb);

  s_instances_.insert({_window, this});
}

glfw_app::glfw_app() : glfw_app([] {}, [] {}) {}

glfw_app::~glfw_app() { glfwTerminate(); }

void glfw_app::run() {
  run(_draw_proc);
}

void glfw_app::run(std::function<void()> draw)
{
  _setup_proc();

  int width, height;
  glfwGetFramebufferSize(_window, &width, &height);
  _fbsize_proc(width, height);

  while (!glfwWindowShouldClose(_window)) {
    draw();
    glfwSwapBuffers(_window);
    glfwPollEvents();
  }
}

void glfw_app::set_fbsize_proc(std::function<void(int width, int height)> cb) {
  _fbsize_proc = cb;
}

void glfw_app::set_key_proc(std::function<void(int k, int, int a, int q)> cb) {
  _key_proc = cb;
}

void glfw_app::set_cursor_proc(std::function<void(double x, double y)> cb) {
  _cursor_proc = cb;
}

void glfw_app::onkey(int k, std::function<void()> cb) {
  LOG(INFO) << "adding keypress handler for " << (char)k;
  _onkey_procs[k] = cb;
}

void glfw_app::s_fbsizeCb(GLFWwindow* win, int width, int height) {
  glfw_app::s_instances_[win]->_fbsize_proc(width, height);
}

void glfw_app::s_keyCb(GLFWwindow* win, int k, int q2, int a, int q) {
  glfw_app::s_instances_[win]->_key_proc(k, q2, a, q);

  auto procs = glfw_app::s_instances_[win]->_onkey_procs;
  auto proc = procs.find(k);
  if (proc != procs.end() && a == GLFW_PRESS) {
    proc->second();
  }
}

void glfw_app::s_mouseCb(GLFWwindow* win, double x, double y) {
  glfw_app::s_instances_[win]->_cursor_proc(x, y);
}

