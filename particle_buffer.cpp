//
// Created by James Anderson on 9/29/16.
//

#include "particle_buffer.h"
#include "vertex_format.h"

using namespace gl;

particle_buffer::particle_buffer(int field_width) {

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  area = field_width * field_width;

  orig_points = new vert[area];
  point_data = new vert[area];
  feedback_buffer = new fb_vert[area];

  for (int y = 0; y < field_width; y++) {
    for (int x = 0; x < field_width; x++) {
      glm::vec2 position = {(1.5 / field_width) * x - 0.7f, (1.5 / field_width) * y - 0.7f};
      point_data[field_width * y + x] = {position, {0.0, 0.0}, position};
      orig_points[field_width * y + x] = {position, {0.0, 0.0}, position};
    }
  }

  glGenBuffers(1, &in_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, in_vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vert) * area, point_data, GL_STREAM_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)sizeof(glm::vec2));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *)(sizeof(glm::vec2) * 2));

  glGenBuffers(1, &out_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, out_vbo);
  glBufferData(GL_ARRAY_BUFFER, area * sizeof(fb_vert), nullptr, GL_STATIC_READ);

  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, out_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, in_vbo);
}

void particle_buffer::draw() {
  glBeginTransformFeedback(GL_POINTS);
  glDrawArrays(GL_POINTS, 0, area);
  glEndTransformFeedback();

  // feed vertex output back into input
  glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0,
                     sizeof(fb_vert) * area, feedback_buffer);
  for (int i = 0; i < area; i++) {
    point_data[i].position = feedback_buffer[i].outPosition;
    point_data[i].velocity = feedback_buffer[i].outVelocity;
  }
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vert) * area, point_data);

}

void particle_buffer::seed() {

  glBufferData(GL_ARRAY_BUFFER, sizeof(vert) * area, orig_points, GL_STREAM_DRAW);
}