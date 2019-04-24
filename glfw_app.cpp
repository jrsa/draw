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

  _window = glfwCreateWindow(800, 600, "", nullptr, nullptr);

  if (!_window) {
    LOG(FATAL) << "failed to create window";
  }
  glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  // XXX: test, this gets called when controllers are plugged in and unplugged
  set_joy_proc([] (int a, int b) { LOG(WARNING) << a << " " << b; });
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

    int count;
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);

    LOG(INFO) << "a" << ": " << (int)buttons[0];
    LOG(INFO) << "b" << ": " << (int)buttons[1];
    LOG(INFO) << "sel" << ": " << (int)buttons[8];
    LOG(INFO) << "start" << ": " << (int)buttons[9];

    // XXX: this is some bullshittery
    // glfwSwapBuffers(_window);
    usleep(10000);
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

void glfw_app::set_joy_proc(GLFWjoystickfun f)
{
    glfwSetJoystickCallback(f);
}
