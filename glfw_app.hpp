#ifndef glfw_app_h
#define glfw_app_h

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <functional>
#include <map>

class glfw_app {
private:
  GLFWwindow *_window;

  std::function<void()> _draw_proc;
  std::function<void()> _setup_proc;
  std::function<void(int k, int, int a, int q)> _key_proc = [] (int, int, int, int) {};
  std::function<void(int width, int height)> _fbsize_proc = [] (int, int) {};
  std::function<void(double x, double y)> _cursor_proc =    [] (double, double) {};

  std::map<int, std::function<void()>> _onkey_procs;

  static std::map<GLFWwindow*, glfw_app*> s_instances_;

public:
  // the old way
  glfw_app(std::function<void()> draw, std::function<void()> setup);
  ~glfw_app();
  void run();

  // the new way
  glfw_app();
  void run(std::function<void()> draw);

  void set_fbsize_proc(std::function<void(int width, int height)>);
  void set_key_proc(std::function<void(int k, int, int a, int)>);
  void set_cursor_proc(std::function<void(double x, double y)>);

  void onkey(int k, std::function<void()>);

  // pass these to glfw to call, then figure out which glfw_app instance to call
  // based on the window pointer
  static void s_fbsizeCb(GLFWwindow* win, int width, int height);
  static void s_keyCb(GLFWwindow* win, int k, int, int a, int q);
  static void s_mouseCb(GLFWwindow*, double x, double y);
};

#endif
