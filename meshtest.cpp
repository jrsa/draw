#include <glog/logging.h>

#include <glbinding/Binding.h>
#include <gl_shared.hpp>
#include <glfw_app.hpp>
#include <shader.hpp>

#include <OVR.h>
#include <rift_in_action.h>


int h = 0, w = 0;


struct eyedata {
  ovrFovPort fov;
  GLuint vao = 0, vbo = 0, ebo = 0;
  ovrDistortionMesh mesh;
  ovrEyeRenderDesc desc;
  ovrSizei tex_size;
};

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

    ovr_Initialize();
    rift = ovrHmd_Create(0);
    if(!rift) {
      LOG(ERROR) << "running without hmd, creating virtual DK1";
      rift = ovrHmd_CreateDebug(ovrHmd_DK1);
    }

    drawer = new shader("ovrdist");

    glm::vec2 *positions[2] = {nullptr};

    unsigned int distcaps = ovrDistortionCap_Vignette
                            | ovrDistortionCap_TimeWarp
                            | ovrDistortionCap_Chromatic;

    // per eye config
    rift_in_action::for_each_eye([&](ovrEyeType i){
      eyes[i].fov = rift->DefaultEyeFov[i];
      eyes[i].desc = ovrHmd_GetRenderDesc(rift, i, eyes[i].fov);
      eyes[i].tex_size = ovrHmd_GetFovTextureSize(rift, i, eyes[i].fov, 1.0);

      ovrHmd_CreateDistortionMesh(rift, i, eyes[i].fov, distcaps, &eyes[i].mesh);
      positions[i] = new glm::vec2[eyes[i].mesh.VertexCount];

      for (int j = 0; j < eyes[i].mesh.VertexCount; ++j) {
        positions[i][j] = rift_in_action::toGlm(eyes[i].mesh.pVertexData[j].ScreenPosNDC);
      }

      glGenVertexArrays(1, &eyes[i].vao);
      glBindVertexArray(eyes[i].vao);

      glGenBuffers(1, &eyes[i].vbo);
      glBindBuffer(GL_ARRAY_BUFFER, eyes[i].vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * eyes[i].mesh.VertexCount, positions[0], GL_STATIC_DRAW);

      glGenBuffers(1, &eyes[i].ebo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eyes[i].ebo);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER
        , sizeof(unsigned short) * eyes[i].mesh.IndexCount
        , eyes[i].mesh.pIndexData
        , GL_STATIC_DRAW);

//      GLsizei stride = sizeof(ovrDistortionVertex);
//      size_t offset = offsetof(ovrDistortionVertex, ScreenPosNDC);

      // position only ya fuck
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

//      offset = offsetof(ovrDistortionVertex, TanEyeAnglesG);
//      glEnableVertexAttribArray(1);
//      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);

    });
    LOG(INFO) << "initialized some stuff";
  };

  auto draw_proc = [&] {
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    static int frameidx = 0;
    ++frameidx;

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    ovrPosef eyePoses[2];
    ovrHmd_GetEyePoses(rift, frameidx, hmdToEyeOffsets, eyePoses, nullptr);

    drawer->use();
    rift_in_action::for_each_eye([&](ovrEyeType i) {
      drawer->u1i("RightEye", i);
//      drawer->u1f("", eyes.);
      glBindVertexArray(eyes[i].vao);
      glDrawElements(GL_TRIANGLES, eyes[i].mesh.IndexCount, GL_UNSIGNED_SHORT, 0);
    });

    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
  };

  glfw_app gltest(draw_proc, setup_proc);
  gltest.run();

//  glDeleteBuffers(1, &vbo);
//  glDeleteVertexArrays(1, &vao);

  return 0;
}