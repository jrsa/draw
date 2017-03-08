#ifndef model_h
#define model_h

#include "gl_shared.hpp"
class shader;

using namespace gl;

// draws a quad covering the entire screen
class billboard {
  GLuint vbo, vao, attrCoord;

public:
  billboard();
  ~billboard();

  void draw();

};

#endif