#include <glog/logging.h>
#include <glbinding/Binding.h>
#include <gl_shared.hpp>
#include <glfw_app.hpp>
#include <shader.hpp>

#include <OVR.h>
#include <rift_in_action.h>

glm::vec2 test_geo[] = {
  {-0.5f,  0.5f},
  { 0.5f,  0.5f},
  { 0.5f, -0.5f},
  {-0.5f, -0.5f},
};

GLushort test_idxes[] = {
  0, 1, 2,
  2, 3, 0
};

int h = 0, w = 0;

int ibo_n = 0;

int main(int argc, char* argv[]) {
  shader::setdir("/Users/jrsa/code/gl/glsl/");
  shader* drawer = nullptr;

  GLuint vao = 0, vbo = 0, ibo = 0;

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, {"glGetError"});
    glbinding::setAfterCallback([](const glbinding::FunctionCall &call) {
      const auto error = glGetError();
      if (error != GL_NO_ERROR)
        LOG(ERROR) << "error in " << call.function->name() << ": " << std::hex << error;
    });

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    ovr_Initialize();
    ovrHmd h = ovrHmd_CreateDebug(ovrHmd_DK1);

    ovrSizei tex_size[2];
    ovrEyeRenderDesc render_desc[2];
    ovrDistortionMesh mesh[2];
    glm::vec2 *positions[2] = {nullptr};

    // per eye config
    for (int i = 0; i < ovrEye_Count; i++) {
      ovrFovPort fov = h->DefaultEyeFov[i];
      render_desc[i] = ovrHmd_GetRenderDesc(h, (ovrEyeType)i, fov);
      tex_size[i] = ovrHmd_GetFovTextureSize(h, (ovrEyeType)i, fov, 1.0);

      const ovrEyeType eye = h->EyeRenderOrder[i];
      unsigned int distcaps = ovrDistortionCap_Chromatic
                              | ovrDistortionCap_TimeWarp
                              | ovrDistortionCap_Vignette;

      ovrHmd_CreateDistortionMesh(h, eye, fov, distcaps, &mesh[i]);
      positions[i] = new glm::vec2[mesh[i].VertexCount];

      for (int j = 0; j < mesh[i].VertexCount; ++j) {
        positions[i][j] = rift_in_action::toGlm(mesh[i].pVertexData[j].ScreenPosNDC);
      }
    }
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * mesh[0].VertexCount, positions[0], GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER
      , sizeof(unsigned short) * mesh[0].IndexCount
      , mesh[0].pIndexData
      , GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);

    drawer = new shader("passthru", "passthru_red");

    ibo_n = mesh[0].IndexCount;
  };

  auto draw_proc = [&] {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    drawer->use();
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, ibo_n, GL_UNSIGNED_SHORT, 0);
  };

  glfw_app gltest(draw_proc, setup_proc);
  gltest.run();

  glDeleteBuffers(1, &vbo);
  glDeleteVertexArrays(1, &vao);

  return 0;
}