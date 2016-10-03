#ifndef shader_h
#define shader_h

#include <string>
#include "gl_shared.hpp"

using namespace gl;

class shader {
private:
  GLuint _program;
  static std::string dir;

public:
  shader(std::string filename, std::vector<std::string> fbv);
  shader(std::string vs_fn, std::string fs_src, std::vector<std::string> fbv);
  ~shader();

  GLuint program() { return _program; }
  void use();
  void u1f(std::string, float);
  void u2f(std::string, glm::vec2);

  static void setdir(std::string);
};

#endif
