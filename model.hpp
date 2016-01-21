#ifndef model_h
#define model_h

#include "gl_shared.hpp"
class shader;

using namespace gl;

struct model {
  shader *p;
  GLuint vbo, vao, attrCoord;

  model();
  ~model();

  void draw();

};

#endif