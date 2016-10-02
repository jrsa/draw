#include "billboard.hpp"
#include "shader.hpp"

using namespace glm;

vec3 vertices[] = {{-1.0f, -1.0f, 0.0f},
                   {-1.0f,  1.0f, 0.0f},
                   { 1.0f, -1.0f, 0.0f},
                   { 1.0f,  1.0f, 0.0f}

};

billboard::billboard() {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glBindVertexArray(0);
}

billboard::~billboard() {
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}

void billboard::draw() {
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
}