#include <glbinding/Binding.h>
#include <glog/logging.h>
#include <cstdlib> // for rand()

#include "billboard.hpp"
#include "fbo.h"
#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "scene.hpp"
#include "shader.hpp"

scene scn;

billboard* bb = nullptr;
shader* source = nullptr;
shader* dest = nullptr;
shader* dest2 = nullptr;
shader* warp = nullptr;
fbo* filt = nullptr; fbo* filt2 = nullptr;

// probably could all be handled by glfw3 and not as global
// variables in this file
int h = 0, w = 0, frame_index = 0;


void allocate_fbos(int w, int h) {
  //
  // 2 fbo's are used in feedback, with a different shader program being used in
  // each
  //
  if(filt) {
    delete filt;
  }
  if(filt2) {
    delete filt2;
  }
  filt = new fbo(h, w);
  filt2 = new fbo(h, w);
}

void load(scene s) {
  if(source) {
    delete source;
  }
  if (dest) {
    delete dest;
  }
  if (dest2) {
    delete dest2;
  }
  if (warp) {
    delete warp;
  }
  source = new shader(s.src);
  dest = new shader(s.front);
  dest2 = new shader(s.back);
  warp = new shader(s.post);
}

void load_shaders() {
  load(scn);
}

void seed() {
  //
  // draw `source` into `filt2`
  //
  filt2->bind();
  source->use();
  source->u1f("scale", 5.0); // domain scale of noise function
  bb->draw();
}
void seed_garbage(fbo* dest) {
  //
  // initialize an empty renderbuffer and draw its contents into `dest`
  //
  fbo* junk_fbo = new fbo(rand() % 1000, rand() % 1000);
  dest->bind();
  bb->draw();
  delete junk_fbo;
}

int main(int argc, char **argv, char **envp) {
  const char* shader_path = getenv("SHADER_PATH");

  if (shader_path)
  {
      shader::setdir(shader_path);
  }
  else
  {
      LOG(FATAL) << "missing SHADER_PATH environment variable";
  }

  srand(0); // just like in the old days

  std::string default_vs ("passthru_pos");
  std::string back_fs ("boxblur");
  std::string src_fs ("simple_src");
  std::string post_fs ("simple_post");

  std::string front_fs;

  if (argc >= 2) {
    front_fs = argv[1];
  } else {
    LOG(FATAL) << "specify fragment shader name as argument";
  }

  if (argc >= 3) {
    back_fs = argv[2];
  }

  if (argc >= 4) {
    post_fs = argv[3];
  }

  scn.src = std::make_pair(default_vs, src_fs);
  scn.back = std::make_pair(default_vs, back_fs);
  scn.post = std::make_pair(default_vs, post_fs);

  scn.front = std::make_pair(default_vs, front_fs);

  auto setup_proc = [&] {

    bb = new billboard();
    load(scn);

    int width = 640;
    int height = 480;

    glViewport(0, 0, width, height);
    allocate_fbos(width, height);
    dest->u2f("dims", glm::vec2(width, height));
    dest2->u2f("dims", glm::vec2(width, height));
    warp->u2f("dims", glm::vec2(width, height));
    w= width;
    h = height;

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
    warp->use();
    bb->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);

  gltest.set_key_proc([](int k, int, int a, int) {
    if(a == GLFW_PRESS) {
      // map number keys to hue uniform in `source`
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
          warp->u2f("dims", glm::vec2(w, h));
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

  gltest.set_fbsize_proc([](int width, int height) {
    // LOG(WARNING) << "changed window size";
    glViewport(0, 0, width, height);
    allocate_fbos(width, height);
    dest->u2f("dims", glm::vec2(width, height));
    dest2->u2f("dims", glm::vec2(width, height));
    warp->u2f("dims", glm::vec2(width, height));
    w= width;
    h = height;
  });

  gltest.run();
  return 0;
}
