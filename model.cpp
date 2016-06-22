#include "model.hpp"
#include "shader.hpp"

using namespace glm;

vec3 vertices[] = {{-1.0f, -1.0f, 0.0f},
                   {-1.0f,  1.0f, 0.0f},
                   { 1.0f, -1.0f, 0.0f},
                   { 1.0f,  1.0f, 0.0f}

};

model::model() : p(nullptr) {
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

model::~model() {
  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);
}

void model::draw() {
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
  glDisableVertexAttribArray(0);
}