#include <glbinding/Binding.h>
#include <glog/logging.h>

#include <lo/lo.h>
#include <lo/lo_cpp.h>

#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "billboard.hpp"
#include "shader.hpp"
#include "fbo.h"

billboard* bb = nullptr;
shader* source = nullptr;
shader* dest = nullptr;
shader* dest2 = nullptr;
fbo* filt = nullptr; fbo* filt2 = nullptr;

int h = 0, w = 0, frame_index = 0;

void seed() {
  filt2->bind();
  source->use();
  source->u1f("scale", 5.0); // domain scale of noise function
  bb->draw();
}

void seed_garbage(fbo* dest) {
  fbo* junk_fbo = new fbo(690, 690);
  dest->bind();
  bb->draw();
}

void allocate_fbos(int w, int h) {
  filt = new fbo(h, w);
  filt2 = new fbo(h, w);
}

void load_shaders() {
  source = new shader("passthru_pos" ,"abe_src", {});
  dest = new shader("passthru_pos", "abe_dest", {});
  dest2 = new shader("passthru_pos", "abe_dest2", {});
}

int main(int argc, char **argv) {

  shader::setdir("/Users/jrsa/code/gl/glsl/img/");

  auto setup_proc = [] {
    glbinding::Binding::initialize(false);

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, {"glGetError"});
    glbinding::setAfterCallback([](const glbinding::FunctionCall &call) {
      const auto error = glGetError();
      if (error != GL_NO_ERROR)
        LOG(ERROR) << "error in " << call.function->name() << ": " << std::hex << error;
    });

//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_BLEND);

    bb = new billboard();
    load_shaders();

    w = 640*2;
    h = 480*2;

    glViewport(0, 0, 640*2, 480*2);
    allocate_fbos(640*2, 480*2);
    dest->u2f("dims", glm::vec2(640*2, 480*2));
    dest2->u2f("dims", glm::vec2(640*2, 480*2));

    seed();
  };

  //
  // iterate feedback loop between "filt" and "filt2" prototypes
  //
  auto draw_proc = [&] {
    // what does this do ffs
    glActiveTexture(GL_TEXTURE0);

    // draw filt2's contents to filt with shader applied
    dest2->use();
    filt2->bind_tex();
    filt->bind();
    bb->draw();

    // the inverse (filt to filt2)
    dest->use();
    filt->bind_tex();
    filt2->bind();
    bb->draw();

    fbo::unbind_all();

    // draws "filt" fbo contents to eyes
    bb->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);

  gltest.set_key_proc([](GLFWwindow *window, int k, int, int a, int) {
    if(a == GLFW_PRESS) {
      if(k >= 48 && k <= 57) {
        int code = k - 48;
        source->u1f("hue", k/10.0);
      }
      switch (k) {
        case 'R': {
          LOG(INFO) << "reloading shader(s)";
          load_shaders();
          dest->u2f("dims", glm::vec2(w, h));
          dest2->u2f("dims", glm::vec2(w, h));
          break;
        }
        case 'S': {
          seed();
          break;
        }
        case 'G': {
          seed_garbage(filt2);
          break;
        }
        default: {
          break;
        }
      }
    }
  });

  gltest.set_fbsize_proc([](GLFWwindow* window, int width, int height) {
    // LOG(WARNING) << "changed window size";
    glViewport(0, 0, width, height);
    allocate_fbos(width, height);
    dest->u2f("dims", glm::vec2(width, height));
    dest2->u2f("dims", glm::vec2(width, height));
    w= width;
    h = height;
  });

  gltest.run();
  return 0;
}
