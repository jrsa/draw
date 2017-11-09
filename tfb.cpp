#include <glbinding/Binding.h>
#include <glog/logging.h>

#include <chrono>

#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "shader.hpp"
#include "particle_buffer.h"
#include "billboard.hpp"
#include "fbo.h"

#include <cstdlib> // atoi?

shader* particle_fb = nullptr;
particle_buffer* particle = nullptr;

int h = 0, w = 0;
bool clear = false;

std::string shader_fn;

fbo* accum = nullptr;
billboard* bb = nullptr;
shader* slab_pass = nullptr;

void load_shaders() {
  particle_fb = new shader(shader_fn, shader_fn, {"outPosition", "outVelocity"});
  slab_pass = new shader("img/passthru");
}

int main(int argc, char **argv) {
  shader::setdir("/Users/jrsa/code/gl/glsl/");
  auto t_prev = std::chrono::high_resolution_clock::now();

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    w = 640;
    h = 480;
    glViewport(0, 0, 640, 480);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    int n = 0;
    if (argc >= 2) {
      n = atoi(argv[1]);
    } else {
      n = 100;
    }

    if (argc >= 3) {
      shader_fn = argv[2];
    }

    load_shaders();

    bb = new billboard();
    accum = new fbo(h, w);

    particle = new particle_buffer(n);
  };

  auto draw_proc = [&] {
    auto t_now = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
    t_prev = t_now;

    glClear(GL_COLOR_BUFFER_BIT);

    // draw particles into fbo
    accum->bind();
    // glViewport(0, 0, w, h);
    if (clear) { 
      glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );
    }
    particle_fb->use();
    particle_fb->u1f("time", time);
    particle->draw();

    fbo::unbind_all();

    // glViewport(0, 0, w, h);
    accum->bind_tex();
    slab_pass->use();
    bb->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);
  gltest.set_key_proc([](GLFWwindow *window, int k, int, int a, int) {
    if(a == GLFW_PRESS) {
      switch (k) {
        case 'C': {
          clear = !clear;
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
    particle_fb->u2f("mousePos", glm::vec2(1/x, 1/y));
  });

  gltest.set_fbsize_proc([](GLFWwindow* window, int width, int height) {
      w = width;
      h = height;
      glViewport(0, 0, w, h);
      accum = new fbo(h, w);
  });

  gltest.run();
  return 0;
}
