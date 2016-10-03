#include <glbinding/Binding.h>
#include <glog/logging.h>

#include <chrono>

#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "shader.hpp"
#include "particle_buffer.h"

shader* particle_fb = nullptr;
particle_buffer* particle = nullptr;

int h = 0, w = 0;
float mouseX = 0, mouseY = 0;

void load_shaders() {
  particle_fb = new shader("xformFb1", {"outPosition", "outVelocity"});
}

int main(int argc, char **argv) {
  shader::setdir("/Users/jrsa/code/gl/glsl/");
  auto t_prev = std::chrono::high_resolution_clock::now();

  auto setup_proc = [] {
    glbinding::Binding::initialize(false);
    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, {"glGetError"});
    glbinding::setAfterCallback([](const glbinding::FunctionCall &call) {
      const auto error = glGetError();
      if (error != GL_NO_ERROR)
        LOG(ERROR) << "error in " << call.function->name() << ": " << std::hex << error;
    });

    load_shaders();

    w = 640*2;
    h = 480*2;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glViewport(0, 0, 640*2, 480*2);

    glPointSize(2.0);
    particle = new particle_buffer(100);
  };

  auto draw_proc = [&] {
    auto t_now = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration_cast<std::chrono::duration<float>>(t_now - t_prev).count();
    t_prev = t_now;

    glActiveTexture(GL_TEXTURE0);
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    particle_fb->use();
    particle_fb->u1f("time", time);
    particle_fb->u2f("mousePos", glm::vec2(mouseX, mouseY));
    particle->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);
  gltest.set_key_proc([](GLFWwindow *window, int k, int, int a, int) {
    if(a == GLFW_PRESS) {
      switch (k) {
        case 'R': {
          LOG(INFO) << "reloading shader(s)";
          load_shaders();
          break;
        }
        case 'S': {
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
    mouseX = x/400.0-1;
    mouseY = y/400.0-1;
  });
  gltest.run();
  return 0;
}
