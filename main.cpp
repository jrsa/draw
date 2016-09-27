#include <glbinding/Binding.h>
#include <glog/logging.h>

#include <rtaudio/RtAudio.h>

#include <lo/lo.h>
#include <lo/lo_cpp.h>

#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "billboard.hpp"
#include "shader.hpp"
#include "fbo.h"

#define SRC_FN "/Users/jrsa/code/gl/draw/glsl/3"
#define DEST_FN "/Users/jrsa/code/gl/draw/glsl/dest"

billboard* bb = nullptr;
shader* source = nullptr;
shader* dest = nullptr;
shader* dest2 = nullptr;
fbo* filt = nullptr; fbo* filt2 = nullptr;

int h = 0, w = 0;

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


int main(int argc, char **argv) {

  shader::setdir("/Users/jrsa/code/gl/draw/glsl/");

  lo::ServerThread oscin(6969);
  DLOG_ASSERT(oscin.is_valid());

  oscin.add_method("/song", "i", [] (lo_arg **pArg, int) {
                           LOG(INFO) << "song " << pArg[0]->i;
                         });

  oscin.add_method("/chords", "i", [] (lo_arg **pArg, int) {
                           LOG(INFO) << "chords "<< pArg[0]->i;
                         });
  oscin.start();
  auto setup_proc = [] {
    glbinding::Binding::initialize(false);

    bb = new billboard();
    source = new shader("3");
    dest = new shader("dest");
    dest2 = new shader("dest2");

    glGenTextures(1, &audiosrc_tex);

    w = 640*2;
    h = 480*2;

    glViewport(0, 0, 640*2, 480*2);
    allocate_fbos(640*2, 480*2);
    dest->u2f("dims", glm::vec2(640*2, 480*2));
    dest2->u2f("dims", glm::vec2(640*2, 480*2));

    seed();

    setup_audiosrc(audiosrc);

//    audiosrc.startStream();
  };

  auto draw_proc = [&] {
    glActiveTexture(GL_TEXTURE0);

    dest2->use();
    filt2->bind_tex();
    filt->bind();
    bb->draw();

    dest->use();
    filt->bind_tex();
    filt2->bind();
    bb->draw();

    fbo::unbind_all();
    bb->draw();
  };

  glfw_app gltest(draw_proc, setup_proc);

  gltest.set_key_proc([](GLFWwindow *window, int k, int, int a, int) {
    if(a == GLFW_PRESS) {
      switch (k) {
        case 'R': {
          LOG(INFO) << "reloading shader(s)";
          source = new shader("3");
          dest = new shader("dest");
          dest2 = new shader("dest2");
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
  return 0;
}
