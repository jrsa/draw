#ifndef glfw_app_h
#define glfw_app_h

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <functional>

class glfw_app {
private:
  GLFWwindow *_window;

public:
  glfw_app(std::function<void()> draw, std::function<void()> setup);
  ~glfw_app();

  void run();
  void set_key_proc(GLFWkeyfun);
  void set_fbsize_proc(GLFWframebuffersizefun);
  void set_cursor_proc(GLFWcursorposfun);
  std::function<void()> draw_proc;
  std::function<void()> setup_proc;
};

#endif