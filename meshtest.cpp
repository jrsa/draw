#include <glog/logging.h>
#include <glbinding/Binding.h>
#include <gl_shared.hpp>
#include <glfw_app.hpp>
#include <shader.hpp>

#include <OVR.h>
#include <rift_in_action.h>

int h = 0, w = 0;

struct hmd_renderer {

};

struct dist_mesh {

  int ibo_n = 0;
  GLuint vao, vbo = 0, ibo = 0;

  ovrSizei tex_size_;
  ovrEyeRenderDesc render_desc_;
  ovrDistortionMesh mesh_;
  glm::vec2 *positions_;
  
  // which eye is this for?
  ovrEyeType eye_;

  dist_mesh(ovrEyeType eye) : eye_(eye), positions_(nullptr) {
  }
   
  ~dist_mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    delete[] positions_;
  }

  void initialize(const ovrHmd& h) {
    LOG(INFO) << "initializing distortion mesh for eye " << eye_;
    ovrFovPort fov = h->DefaultEyeFov[eye_];

    render_desc_ = ovrHmd_GetRenderDesc(h, eye_, fov);
    tex_size_ = ovrHmd_GetFovTextureSize(h, eye_, fov, 1.0);

    // const ovrEyeType eye = h->EyeRenderOrder[eye_];
    const ovrEyeType eye = eye_;

    unsigned int distcaps = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp | ovrDistortionCap_Vignette;

    ovrHmd_CreateDistortionMesh(h, eye, fov, distcaps, &mesh_);

    positions_ = new glm::vec2[mesh_.VertexCount];

    for (int j = 0; j < mesh_.VertexCount; ++j)
      positions_[j] = rift_in_action::toGlm(mesh_.pVertexData[j].ScreenPosNDC);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * mesh_.VertexCount, positions_, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * mesh_.IndexCount, mesh_.pIndexData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);

    ibo_n = mesh_.IndexCount;
  }

  void draw() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, ibo_n, GL_UNSIGNED_SHORT, 0);
  }
};


int main(int argc, char* argv[]) {
  shader::setdir("/Users/jrsa/code/gl/glsl/");
  shader* drawer = nullptr;

  GLuint vao = 0;

  // dist_mesh distmeshes[ovrEye_Count];
  dist_mesh left(ovrEye_Left);
  dist_mesh right(ovrEye_Right);

  auto setup_proc = [&] {
    glbinding::Binding::initialize(false);

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, {"glGetError"});
    glbinding::setAfterCallback([](const glbinding::FunctionCall &call) {
      const auto error = glGetError();
      if (error != GL_NO_ERROR)
        LOG(ERROR) << "error in " << call.function->name() << ": " << std::hex << error;
    });

    ovr_Initialize();

    // iirc `ovrHmd_Create(0)` will use a a real unit, if available, and fail if it is not
    ovrHmd h = ovrHmd_CreateDebug(ovrHmd_DK1);

    left.initialize(h);
    right.initialize(h);

    drawer = new shader("passthru", "passthru_red");
  };

  auto draw_proc = [&] {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // dummy shader that just colors everything red
    drawer->use();

    left.draw();
    right.draw();
  };

  glfw_app gltest(draw_proc, setup_proc);
  gltest.run();

  return 0;
}
