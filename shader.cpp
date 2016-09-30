#include <string>
#include <vector>
#include <fstream>
#include <glog/logging.h>

#include "shader.hpp"
#include "simple_file.hpp"

using namespace gl;
using namespace simple_file;

void compile_info(const GLuint shader);
void link_info(const GLuint shader);
void load_shader(const GLuint shader, std::string fn);
void variable_info(const GLuint program);

shader::shader(std::string vs_fn, std::string fs_fn) {
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

  load_shader(vs, dir + vs_fn + ".vs.glsl");
  load_shader(fs, dir + fs_fn + ".fs.glsl");

  glCompileShader(vs);
  compile_info(vs);

  glCompileShader(fs);
  compile_info(fs);

  _program = glCreateProgram();

  glAttachShader(_program, vs);
  glAttachShader(_program, fs);
  glLinkProgram(_program);
  link_info(_program);

  variable_info(_program);
}

shader::shader(std::string filename): shader(filename, filename) { }

shader::~shader() { glDeleteProgram(_program); }

void shader::use() { glUseProgram(_program); }

void shader::u1f(std::string name, float value) {
  use();
  GLint u = glGetUniformLocation(_program, name.c_str());
  glUniform1f(u, value);
}

void shader::u2f(std::string name, glm::vec2 value) {
  use();
  GLint u = glGetUniformLocation(_program, name.c_str());
  glUniform2f(u, value.x, value.y);
}

std::string shader::dir;

void shader::setdir(std::string d) {
  dir = d;
}

void compile_info(const GLuint shader) {
  GLint status(0);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  if (1 != status) {
    GLint maxLength(0);
    GLint logLength(0);

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    GLchar *log = new GLchar[maxLength];
    glGetShaderInfoLog(shader, maxLength, &logLength, log);

    LOG(FATAL) << "glsl compile error: " << log;
  }
}

void link_info(const GLuint program) {
  GLint status(0);
  glGetProgramiv(program, GL_LINK_STATUS, &status);

  if (1 != status) {
    GLint maxLength(0);
    GLint logLength(0);

    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    GLchar *log = new GLchar[maxLength];
    glGetProgramInfoLog(program, maxLength, &logLength, log);

    LOG(FATAL) << "glsl link error: " << log;
  }
}

void load_shader(const GLuint shader, std::string fn) {
  std::ifstream srcfile(fn, std::ios::in | std::ios::binary);

  if (!srcfile.is_open()) {
    LOG(FATAL) << "couldn't open shader: " << fn;
  }
  srcfile.seekg(0, std::ios::end);
  int sz = srcfile.tellg();
  char *buf = new char[sz + 1];

  srcfile.seekg(0, std::ios::beg);
  srcfile.read(buf, sz);

  buf[sz] = '\0'; // ensure null terminator
  glShaderSource(shader, 1, &buf, NULL);
  delete[] buf;
}

void variable_info(const GLuint program) {
  GLint n_active_attr = 0;
  GLint n_active_ufrm = 0;
  GLint max_attr_namelength = 0;
  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &n_active_attr);
  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &n_active_ufrm);
  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
                 &max_attr_namelength);

  LOG(INFO) << n_active_attr << " attrs and " << n_active_ufrm << " ufrms";

  std::vector<GLchar> nameData(max_attr_namelength);
  for (int attr = 0; attr < n_active_attr; ++attr) {
    GLint arraySize = 0;
    GLenum type = GL_FLOAT;
    GLsizei actualLength = 0;
    glGetActiveAttrib(program, attr, nameData.size(), &actualLength,
                      &arraySize, &type, &nameData[0]);

    LOG(INFO) << std::string((char *)&nameData[0], actualLength);
  }
}
