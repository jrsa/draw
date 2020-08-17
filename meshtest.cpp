#include <glog/logging.h>
#include <glbinding/Binding.h>
#include <gl_shared.hpp>
#include <glfw_app.hpp>
#include <shader.hpp>
#include <fbo.h>

#include <glm/gtc/matrix_transform.hpp>
#include <OVR.h>
#include <rift_in_action.h>

#include <box.hpp>


struct dist_mesh {

  int ibo_n = 0;
  GLuint vao, vbo = 0, ibo = 0;

  ovrSizei tex_size_;
  ovrEyeRenderDesc render_desc_;
  ovrDistortionMesh mesh_;

  // scale, offset?? fucking docs dont even say
  ovrVector2f uvScaleOffset_[2];

  // which eye is this for?
  ovrEyeType eye_;

  dist_mesh(ovrEyeType eye) : eye_(eye) {
  }
   
  ~dist_mesh() {
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
  }

  void initialize(const ovrHmd& h) {
    LOG(INFO) << "initializing distortion mesh for eye " << eye_;
    ovrFovPort fov = h->DefaultEyeFov[eye_];

    render_desc_ = ovrHmd_GetRenderDesc(h, eye_, fov);
    tex_size_ = ovrHmd_GetFovTextureSize(h, eye_, fov, 1.0);

    ovrSizei textureSize;
    ovrRecti renderViewport;

    textureSize.w = 1024;
    textureSize.h = 1024;

    renderViewport.Size.w = textureSize.w;
    renderViewport.Size.h = textureSize.h;
    renderViewport.Pos.x = 0;
    renderViewport.Pos.y = 0;

    ovrHmd_GetRenderScaleAndOffset(fov, textureSize, renderViewport, uvScaleOffset_);

    // const ovrEyeType eye = h->EyeRenderOrder[eye_];
    const ovrEyeType eye = eye_;

    unsigned int distcaps = ovrDistortionCap_Chromatic | ovrDistortionCap_TimeWarp | ovrDistortionCap_Vignette;


    ovrHmd_CreateDistortionMesh(h, eye, fov, distcaps, &mesh_);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ovrDistortionVertex_) * mesh_.VertexCount, mesh_.pVertexData, GL_STATIC_DRAW);

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * mesh_.IndexCount, mesh_.pIndexData, GL_STATIC_DRAW);

    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ovrDistortionVertex_), 0);

    // texcoord? tanEyeAngle? watf
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ovrDistortionVertex_), (const void *)32);

    glBindVertexArray(0);

    ibo_n = mesh_.IndexCount;

#if 0
    for (int i = 0; i < 30; i++)
        LOG(INFO) 
            << mesh_.pVertexData[i].TanEyeAnglesR.x
            << mesh_.pVertexData[i].TanEyeAnglesR.y;
#endif
  }

  void draw() {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, ibo_n, GL_UNSIGNED_SHORT, 0);
  }
};


int main(int argc, char* argv[]) {
  const char* shader_path = getenv("SHADER_PATH");

  if (shader_path)
  {
      shader::setdir(shader_path);
  }
  else
  {
      LOG(FATAL) << "missing SHADER_PATH environment variable";
  }

  box b;

  shader* dist_shader = nullptr;
  shader* hsvbox = nullptr;

  // XXX: pls dont commit it like this ugh fuck
  fbo* left_eye = nullptr;
  fbo* right_eye = nullptr;

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

    glEnable(GL_DEPTH_TEST); // always
    glClearColor(0.0, 0.0, 0.0, 1.0);

    DLOG_ASSERT(ovr_Initialize());
    LOG(INFO) << "libovr" << ovr_GetVersionString();

    // iirc `ovrHmd_Create(0)` will use a a real unit, if available, and fail if it is not

    LOG(INFO) << "looking for hmd hardware: " << ovrHmd_Detect();

    ovrHmd h = ovrHmd_Create(0);

    if (!h) {
        LOG(WARNING) << "no hmd hardware found, creating simulated/dbg hmd instance";
        h = ovrHmd_CreateDebug(ovrHmd_DK1);
    } else {
        LOG(INFO) << "found real live oculus rift" << h->ProductName;
    }

    // initialize per eye distortion meshes, this does some openGL setup, so the calls have to happen after the openGL
    // context is created

    left.initialize(h);
    right.initialize(h);

    // XXX: also pls dont commit it like this ugh fuck
    dist_shader = new shader("vr/ovrdist", "vr/ovrdist");
    dist_shader->u2f("dim", glm::vec2(1024, 1024));
    hsvbox = new shader("mesh/basic3d", "mesh/hsv3d");
    left_eye = new fbo(1024, 1024);
    right_eye = new fbo(1024, 1024);

    b.init();
  };

  auto draw_proc = [&] {

    // compute matrix stack, there should be a common m and v matrix, then a 
    // translated v and a p for each eye (i think)

    hsvbox->use();

    mat4 model;
    model = rotate(model, (float) glfwGetTime() * radians(10.0f), vec3(0.0f, 0.0f, 1.0f));
    model = scale(model, vec3(0.5, 0.5, 0.5));
    mat4 view = lookAt(vec3(1.2f, 1.2f, 1.2f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));

    // TODO: populate this with sensical values that come from the oculus library
    mat4 proj = perspective(radians(55.0f), 800.0f / 600.0f, 0.10f, 100.0f);

    hsvbox->u44m("model", model);
    hsvbox->u44m("view", view);
    hsvbox->u44m("proj", proj);

    left_eye->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    b.draw();

    right_eye->bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    b.draw();

    fbo::unbind_all();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    dist_shader->use();
    dist_shader->u1i("RightEye", 0);
    
    // TODO: move this into the draw method of the distortion mesh, have to pass 
    // the shader in there somehow, to set the uniform...
    dist_shader->u4f("uvScaleAndOffset", 
        glm::vec4( 
            left.uvScaleOffset_[0].x, 
            left.uvScaleOffset_[0].y,
            left.uvScaleOffset_[1].x, 
            left.uvScaleOffset_[1].y
        )
    );

    left_eye->bind_tex();
    left.draw();

    dist_shader->u1i("RightEye", 1);
    dist_shader->u4f("uvScaleAndOffset", 
        glm::vec4( 
            right.uvScaleOffset_[0].x, 
            right.uvScaleOffset_[0].y,
            right.uvScaleOffset_[1].x, 
            right.uvScaleOffset_[1].y
        )
    );

    right_eye->bind_tex();
    right.draw();
  };

  glfw_app gltest(draw_proc, setup_proc);
  gltest.run();

  return 0;
}
