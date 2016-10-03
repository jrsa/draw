//
// Created by James Anderson on 5/1/16.
//

#ifndef PROJECT_VERTEX_FORMAT_H
#define PROJECT_VERTEX_FORMAT_H

#define GLM_COMPILER 0
#include <glm/glm.hpp>

struct vert {
  glm::vec2 position;
  glm::vec2 velocity;
  glm::vec2 originalPos;
};

struct fb_vert {
  glm::vec2 outPosition;
  glm::vec2 outVelocity;
};

#endif // PROJECT_VERTEX_FORMAT_H
