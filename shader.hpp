#ifndef shader_h
#define shader_h

#include <string>
#include <vector>
#include "gl_shared.hpp"
#include "scene.hpp"

using namespace gl;

class shader {
private:
  GLuint _program;
  static std::string dir;
  std::string _vname, _fname;

public:
  shader(std::string filename, std::vector<std::string> fbv);
  shader(std::string vs_fn, std::string fs_fn, std::vector<std::string> fbv);
  shader(std::string filename);
  shader(std::string vs_fn, std::string fs_fn);
  shader(std::pair<std::string, std::string>);
  ~shader();

  GLuint program() { return _program; }
  void use();
  void u1f(std::string, float);
  void u2f(std::string, glm::vec2);
  void u3f(std::string, glm::vec3);
  void u4f(std::string, glm::vec4);
  void u1i(std::string, GLint);

  void u44m(std::string, glm::mat4);

  void reload();

  static void setdir(std::string);
};

#endif
