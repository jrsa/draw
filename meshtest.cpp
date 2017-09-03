#include <glog/logging.h>

#include <glbinding/Binding.h>
#include <gl_shared.hpp>
#include <glfw_app.hpp>
#include <billboard.hpp>
#include <fbo.h>
#include <shader.hpp>

#include <OVR.h>
#include <rift_in_action.h>

shader *dist = nullptr;
shader* source = nullptr;
shader* pass = nullptr;

void load_shaders() {
  // test image slab
  source = new shader("img/passthru_pos" ,"img/simple_src");

  // rift distortion
  dist = new shader("vr/ovrdist", "vr/ovrdist");

  // passthru
  pass = new shader("img/passthru", "img/passthru");
}

int main(int argc, char *argv[]) {
  shader::setdir("/Users/jrsa/code/gl/glsl/");
  fbo *slab = nullptr;
  billboard *bb = nullptr;

  GLuint vao[2], vbo[2], ibo[2];
  ovrSizei tex_size[2];
  ovrEyeRenderDesc render_desc[2];
  ovrDistortionMesh mesh[2];
  glm::vec2 *positions[2] = {nullptr};

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, {"glGetError"});
    glbinding::setAfterCallback([](const glbinding::FunctionCall &call) {
      const auto error = glGetError();
      if (error != GL_NO_ERROR) {
        LOG(ERROR) << "error in " << call.function->name() << ": " << std::hex << error;
      }
    });


    // openGL element array meshes for each eye
    glGenVertexArrays(2, vao);
    glGenBuffers(2, vbo);
    glGenBuffers(2, ibo);

    // rift things
    ovr_Initialize();
    ovrHmd h = ovrHmd_CreateDebug(ovrHmd_DK1);

    // per eye config
    for (int i = 0; i < ovrEye_Count; i++) {
      glBindVertexArray(vao[i]);
      ovrFovPort fov = h->DefaultEyeFov[i];
      render_desc[i] = ovrHmd_GetRenderDesc(h, (ovrEyeType)i, fov);
      tex_size[i] = ovrHmd_GetFovTextureSize(h, (ovrEyeType)i, fov, 1.0);

      const ovrEyeType eye = h->EyeRenderOrder[i];
      unsigned int distcaps = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp | ovrDistortionCap_Vignette;

      ovrHmd_CreateDistortionMesh(h, eye, fov, distcaps, &mesh[i]);
      positions[i] = new glm::vec2[mesh[i].VertexCount];

      for (int j = 0; j < mesh[i].VertexCount; ++j) {
        positions[i][j] = rift_in_action::toGlm(mesh[i].pVertexData[j].ScreenPosNDC);
      }

      glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * mesh[i].VertexCount, mesh[i].pVertexData, GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo[i]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * mesh[i].IndexCount, mesh[i].pIndexData, GL_STATIC_DRAW);

      glEnableVertexAttribArray(0); // hardcoded in ovrdist.vs.glsl

      // specify tightly packed vec2s for position
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
      glBindVertexArray(0);
    }

    // for source image
    bb = new billboard();
    slab = new fbo(512, 512);
    load_shaders();    
  };

  auto draw_proc = [&] {
    // glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw test pattern into texture
    slab->bind();
    source->use();
    bb->draw();

    // switch back to drawing to screen
    fbo::unbind_all();

    // slab->bind_tex();
    // pass->use();
    // bb->draw();

    // in the future: ovr->drawEyes(l, r, shader) or similar
    
    dist->use();
    slab->bind_tex();
    glBindVertexArray(vao[0]);
    glDrawElements(GL_TRIANGLES, mesh[0].IndexCount, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(vao[1]);
    glDrawElements(GL_TRIANGLES, mesh[1].IndexCount, GL_UNSIGNED_SHORT, 0);
    
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
        default: {
          break;
        }
      }
    }
  });
  gltest.run();

  glDeleteBuffers(2, vbo);
  glDeleteVertexArrays(2, vao);

  return 0;
}