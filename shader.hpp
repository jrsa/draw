#ifndef shader_h
#define shader_h

#include <string>
#include "gl_shared.hpp"

using namespace gl;

class shader {
private:
  GLuint _program;

public:
  shader(std::string filename);
  ~shader();

  void use();
};

#endif
