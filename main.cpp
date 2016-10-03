#include <glbinding/Binding.h>
#include <glog/logging.h>

#include <rtaudio/RtAudio.h>

#include <lo/lo.h>
#include <lo/lo_cpp.h>

#include <OVR.h>
#include "hmd_dist_mesh.h"

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

// class rift
ovrHmd rifthmd;
shader* distortion = nullptr;

// per eye
ovrTexture eyes[ovrEye_Count];
ovrEyeRenderDesc eye_render_desc[ovrEye_Count];
ovrMatrix4f eye_proj[ovrEye_Count];          // Projection matrix for eye.
ovrMatrix4f eye_ortho[ovrEye_Count];     // Projection for 2D.
ovrPosef eye_render_pose[ovrEye_Count];       // Poses we used for rendering.
ovrSizei eye_render_size[ovrEye_Count];       // Saved render eye sizes; base for dynamic sizing.
GLuint eye_tex[ovrEye_Count];
fbo* eye_dest[ovrEye_Count] = {nullptr};
hmd_dist_mesh* eye_mesh[ovrEye_Count] = {nullptr};

// class rtaudiosrc
RtAudio audiosrc;
signed short *inbuffer = nullptr;
GLuint audiosrc_tex;

void seed() {
  filt2->bind();
  source->use();
  source->u1f("scale", 5.0); // domain scale of noise function
  bb->draw();
}

void allocate_fbos(int w, int h) {
  filt = new fbo(h, w);
  filt2 = new fbo(h, w);
}

void load_shaders() {

  source = new shader("passthru_pos" ,"3", {});
  dest = new shader("passthru_pos", "dest", {});
  dest2 = new shader("passthru_pos", "dest2", {});
}

int record( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
            double streamTime, RtAudioStreamStatus status, void *userData ) {
//  glBindTexture(GL_TEXTURE_1D, audiosrc_tex);
//  glTexImage1D(GL_TEXTURE_1D, 0, (GLint)GL_R16, nBufferFrames, 0, GL_RED, GL_UNSIGNED_SHORT, inputBuffer);
  memccpy(inbuffer, inputBuffer, nBufferFrames, sizeof(short));
}


void setup_audiosrc(RtAudio& src) {

  LOG(INFO) << src.getDeviceCount() << " devices found";
  unsigned int bufferFrames = 768;
  RtAudio::StreamParameters parameters;
  parameters.nChannels = 1;

  inbuffer = new short[bufferFrames];

  src.openStream( NULL, &parameters, RTAUDIO_SINT16, 48000, &bufferFrames, &record );
  LOG(INFO) << "stream opened with vector size: " << bufferFrames;
}

void setup_rift() {
  DLOG_ASSERT(ovr_Initialize());
  LOG(INFO) << "libovr " << ovr_GetVersionString() << " blud";
  rifthmd = ovrHmd_Create(0);
  //  DLOG_ASSERT(rifthmd);
  if(!rifthmd) {
    LOG(ERROR) << "running without hmd, creating virtual DK1";
    rifthmd = ovrHmd_CreateDebug(ovrHmd_DK1);
  }

  ovrSizei tex_size[2];
  ovrEyeRenderDesc render_desc[2];

  // per eye config
  for (int i = 0; i < ovrEye_Count; i++) {
    ovrFovPort fov = rifthmd->DefaultEyeFov[i];
    render_desc[i] = ovrHmd_GetRenderDesc(rifthmd, (ovrEyeType)i, fov);
    tex_size[i] = ovrHmd_GetFovTextureSize(rifthmd, (ovrEyeType)i, fov, 1.0);

    const ovrEyeType eye = rifthmd->EyeRenderOrder[i];
    unsigned int distcaps = ovrDistortionCap_Chromatic
                          | ovrDistortionCap_TimeWarp
                          | ovrDistortionCap_Vignette;
    eye_mesh[i] = new hmd_dist_mesh(rifthmd, (ovrEyeType)i, fov, distcaps);
  }

  // 2 textures or one?
//  glGenTextures()

}


int main(int argc, char **argv) {

  shader::setdir("/Users/jrsa/code/gl/glsl/");

//  lo::ServerThread oscin(6969);
//  DLOG_ASSERT(oscin.is_valid());
//
//  oscin.add_method("/song", "i", [] (lo_arg **pArg, int) {
//                           LOG(INFO) << "song " << pArg[0]->i;
//                         });
//
//  oscin.add_method("/chords", "i", [] (lo_arg **pArg, int) {
//                           LOG(INFO) << "chords "<< pArg[0]->i;
//                         });
//  oscin.start();
  auto setup_proc = [] {
    glbinding::Binding::initialize(false);

    glbinding::setCallbackMaskExcept(glbinding::CallbackMask::After, {"glGetError"});
    glbinding::setAfterCallback([](const glbinding::FunctionCall &call) {
      const auto error = glGetError();
      if (error != GL_NO_ERROR)
        LOG(ERROR) << "error in " << call.function->name() << ": " << std::hex << error;
    });

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    bb = new billboard();
    load_shaders();

//    glGenTextures(1, &audiosrc_tex);

    w = 640*2;
    h = 480*2;

    glViewport(0, 0, 640*2, 480*2);
    allocate_fbos(640*2, 480*2);
    dest->u2f("dims", glm::vec2(640*2, 480*2));
    dest2->u2f("dims", glm::vec2(640*2, 480*2));

    seed();

//    setup_audiosrc(audiosrc);
    setup_rift();
//    audiosrc.startStream();
  };

  auto draw_proc = [&] {

    // iterate feedback loop between "filt" and "filt2" prototypes
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

//    ovrTrackingState hmdstate;
//    ovrVector3f hmd2eyeview_offset[2] = { eye_render_desc[0].HmdToEyeViewOffset
//                                        , eye_render_desc[1].HmdToEyeViewOffset };
//    ovrHmd_GetEyePoses(rifthmd, 0, hmd2eyeview_offset, eye_render_pose, &hmdstate);

    // draws "filt" fbo contents to eyes
    bb->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);

  gltest.set_key_proc([](GLFWwindow *window, int k, int, int a, int) {
    if(a == GLFW_PRESS) {
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
        }
        default: {
          break;
        }
      }
    }
  });

  gltest.set_fbsize_proc([](GLFWwindow* window, int width, int height) {
    LOG(WARNING) << "changed window size";
    glViewport(0, 0, width, height);
    allocate_fbos(width, height);
    dest->u2f("dims", glm::vec2(width, height));
    dest2->u2f("dims", glm::vec2(width, height));
    w= width;
    h = height;
  });

  gltest.run();

  ovrHmd_Destroy(rifthmd);
  ovr_Shutdown();
  return 0;
}
