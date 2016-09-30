#ifndef PROJECT_PARTICLE_BUFFER_H
#define PROJECT_PARTICLE_BUFFER_H


#include "gl_shared.hpp"
#include "vertex_format.h"

class particle_buffer {
public:
  particle_buffer(int field_width);
  void draw();
  void seed();

private:
  int area;

  // host buffers
  vert *orig_points;
  vert *point_data;
  fb_vert *feedback_buffer;

  gl::GLuint vao = 0;
  gl::GLuint in_vbo = 0;
  gl::GLuint out_vbo = 0;
};


#endif
