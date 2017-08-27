#include <glbinding/Binding.h>
#include <glog/logging.h>

#include <chrono>

#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "shader.hpp"
#include "particle_buffer.h"

#include <cstdlib> // atoi?

shader* particle_fb = nullptr;
particle_buffer* particle = nullptr;

int h = 0, w = 0;
float mouseX = 0, mouseY = 0;

void load_shaders() {
  particle_fb = new shader("xformfborig", "xformfborig", {"outPosition", "outVelocity"});
}

int main(int argc, char **argv) {
  shader::setdir("/Users/jrsa/code/gl/glsl/tfb/");
  auto t_prev = std::chrono::high_resolution_clock::now();

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    w = 640;
    h = 480;
    glViewport(0, 0, 640, 480);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    load_shaders();

    int n = 0;
    if (argc >= 2) {
      n = atoi(argv[1]);
    } else {
      n = 100;
    }

    particle = new particle_buffer(n);
  };

  auto draw_proc = [&] {
    auto t_now = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
    t_prev = t_now;

    // glActiveTexture(GL_TEXTURE0);
    // glClearColor(0.0, 0., 0., .1);
    // glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    particle_fb->use();
    particle_fb->u1f("time", time);
    particle_fb->u2f("mousePos", glm::vec2(mouseX, mouseY));
    particle->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);
  gltest.set_key_proc([](GLFWwindow *window, int k, int, int a, int) {
    if(a == GLFW_PRESS) {
      switch (k) {
        case 'C': {
          glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
          break;
        }
        case 'R': {
          load_shaders();
          break;
        }
        case 'P': {
          particle->seed();
          break;
        }
        default: {
          break;
        }
      }
    }
  });
  gltest.set_cursor_proc([](GLFWwindow*,double x, double y){
    mouseX = 1 / x;
    mouseY = 1 / y;
  });
  gltest.run();
  return 0;
}
