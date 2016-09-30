#include "hmd_dist_mesh.h"

using namespace gl;

hmd_dist_mesh::hmd_dist_mesh(ovrHmd hmd
                           , ovrEyeType eye
                           , ovrFovPort fov
                           , unsigned int distcaps) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  ovrDistortionMesh mesh;
  ovrHmd_CreateDistortionMesh(hmd, eye, fov, distcaps, &mesh);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER
    , sizeof(ovrDistortionVertex) * mesh.VertexCount
    , mesh.pVertexData
    , GL_STATIC_DRAW);

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER
    , sizeof(unsigned short) * mesh.IndexCount
    , mesh.pIndexData
    , GL_STATIC_DRAW);

  size_t stride = sizeof(ovrDistortionVertex);
  size_t offset = offsetof(ovrDistortionVertex, ScreenPosNDC);


  ovrHmd_DestroyDistortionMesh(&mesh);
  glBindVertexArray(0);
}

hmd_dist_mesh::~hmd_dist_mesh() {
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}

void hmd_dist_mesh::draw() {
  glBindVertexArray(vao);

  // TODO

  glBindVertexArray(0);
}