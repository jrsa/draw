#ifndef glfw_app_h
#define glfw_app_h

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <functional>

class glfw_app {
private:
  GLFWwindow *_window;

public:
  glfw_app();
  glfw_app(std::function<void()>);
  ~glfw_app();

  void run();
  std::function<void()> draw_proc;
};

#endif