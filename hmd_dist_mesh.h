#ifndef PROJECT_HMD_DIST_MESH_H
#define PROJECT_HMD_DIST_MESH_H

#include "gl_shared.hpp"
#include <OVR.h>

using gl::GLuint;

class hmd_dist_mesh {
  GLuint vbo, vao, ibo;
public:
  hmd_dist_mesh(ovrHmd hmd, ovrEyeType eye, ovrFovPort fov, unsigned int distcaps);
  ~hmd_dist_mesh();

  void draw();
};

#endif