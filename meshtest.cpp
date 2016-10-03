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

struct eyedata {
  ovrFovPort fov;
  GLuint vao = 0, vbo = 0, ebo = 0;
  ovrDistortionMesh mesh;
  ovrEyeRenderDesc desc;
};

int ibo_n = 0;

int main(int argc, char* argv[]) {
  shader::setdir("/Users/jrsa/code/gl/glsl/");
  shader* drawer = nullptr;

  ovrHmd rift;
  eyedata eyes[2];

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, {"glGetError"});
    glbinding::setAfterCallback([](const glbinding::FunctionCall &call) {
      const auto error = glGetError();
      if (error != GL_NO_ERROR)
        LOG(ERROR) << "error in " << call.function->name() << ": " << std::hex << error;
    });

    DLOG_ASSERT(ovr_Initialize());
    LOG(INFO) << "libovr " << ovr_GetVersionString() << " blud";
    rift = ovrHmd_Create(0);
    if(!rift) {
      LOG(ERROR) << "running without hmd, creating virtual DK1";
      rift = ovrHmd_CreateDebug(ovrHmd_DK1);
    }

    drawer = new shader("passthru2d", "passthru_red");


    rift_in_action::for_each_eye([&](ovrEyeType eyei) {
      eyedata eye = eyes[eyei];
      eye.fov = rift->DefaultEyeFov[eyei];

      eye.desc = ovrHmd_GetRenderDesc(rift, eyei, eye.fov);

      unsigned int distcaps = ovrDistortionCap_Chromatic
                              | ovrDistortionCap_TimeWarp
                              | ovrDistortionCap_Vignette;


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


      glGenBuffers(1, &eye.vbo);
      glBindBuffer(GL_ARRAY_BUFFER, eye.vbo);
      glBufferData(GL_ARRAY_BUFFER
        , sizeof(ovrDistortionVertex) * eye.mesh.VertexCount
        , eye.mesh.pVertexData
        , GL_STATIC_DRAW);

      glGenBuffers(1, &eye.ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eye.ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER
        , sizeof(unsigned short) * eye.mesh.IndexCount
        , eye.mesh.pIndexData
        , GL_STATIC_DRAW);

      GLsizei stride = sizeof(ovrDistortionVertex);
      size_t offset = offsetof(ovrDistortionVertex, ScreenPosNDC);

      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);

      offset = offsetof(ovrDistortionVertex, TanEyeAnglesG);
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);

    });

    drawer = new shader("passthru", "passthru_red");

    ibo_n = mesh[0].IndexCount;
  };

  auto draw_proc = [&] {
    glClearColor(1.0, 1.0, 1.0, 1.0);
//    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    drawer->use();
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, ibo_n, GL_UNSIGNED_SHORT, 0);

  };

  glfw_app gltest(draw_proc, setup_proc);
  gltest.run();

//  glDeleteBuffers(1, &vbo);
//  glDeleteVertexArrays(1, &vao);

  return 0;
}