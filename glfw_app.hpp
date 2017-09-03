#ifndef glfw_app_h
#define glfw_app_h

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <functional>

class glfw_app {
private:
  GLFWwindow *_window;
  std::function<void()> _draw_proc;
  std::function<void()> _setup_proc;
  std::function<void()> _key_proc;
  std::function<void()> _fbsize_proc;
  std::function<void()> _cursor_proc;

public:
  glfw_app(std::function<void()> draw, std::function<void()> setup);
  ~glfw_app();

  void run();
  void set_key_proc(GLFWkeyfun);
  void set_fbsize_proc(GLFWframebuffersizefun);
  void set_cursor_proc(GLFWcursorposfun);
};

#endif