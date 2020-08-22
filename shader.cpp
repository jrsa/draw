#include <string>
#include <vector>
#include <fstream>
#include <glog/logging.h>

#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
// #include "simple_file.hpp"

using namespace gl;
// using namespace simple_file;

void compile_info(const GLuint shader);
void link_info(const GLuint shader);
void load_shader(const GLuint shader, std::string fn);
void variable_info(const GLuint program);

std::string error_name ("<not loaded>");

shader::shader(std::string vs_fn, std::string fs_fn, std::vector<std::string> fbv)
  : _vname(error_name), _fname(error_name) {
  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

  LOG(INFO) << "loading: " << vs_fn;
  load_shader(vs, dir + vs_fn + ".vs.glsl");

  LOG(INFO) << "loading: " << fs_fn;
  load_shader(fs, dir + fs_fn + ".fs.glsl");

  glCompileShader(vs);
  compile_info(vs);

  glCompileShader(fs);
  compile_info(fs);

  _program = glCreateProgram();
  glAttachShader(_program, vs);
  glAttachShader(_program, fs);

  if(fbv.size()) {
    std::vector<const GLchar*> varyings;
    for (auto i = fbv.begin(); i != fbv.end(); ++i) {
      varyings.push_back(i->c_str());
    }
    glTransformFeedbackVaryings(_program, 2, &varyings[0], GL_INTERLEAVED_ATTRIBS);
  }

  glLinkProgram(_program);
  link_info(_program);

  _vname = vs_fn;
  _fname = fs_fn;

  variable_info(_program);
}

shader::shader(std::string filename, std::vector<std::string> fbv): shader(filename, filename, fbv) {}
shader::shader(std::string filename): shader(filename, filename, {}) {}
shader::shader(std::string vs_fn, std::string fs_fn): shader(vs_fn, fs_fn, {}) {}

shader::shader(std::pair<std::string, std::string> names): shader(names.first, names.second, {}) {} 

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

void shader::u3f(std::string name, glm::vec3 value) {
  use();
  GLint u = glGetUniformLocation(_program, name.c_str());
  glUniform3f(u, value.x, value.y, value.z);
}

void shader::u4f(std::string name, glm::vec4 value) {
  use();
  GLint u = glGetUniformLocation(_program, name.c_str());
  glUniform4f(u, value.x, value.y, value.z, value.w);
}

void shader::u1i(std::string name, GLint value) {
  use();
  GLint u = glGetUniformLocation(_program, name.c_str());
  glUniform1i(u, value);
}

void shader::u44m(std::string name, glm::mat4 m) {
  use();
  GLint u = glGetUniformLocation(_program, name.c_str());  
  glUniformMatrix4fv(u, 1, GL_FALSE, glm::value_ptr(m));
}

std::string shader::dir;

void shader::setdir(std::string d) {
  dir = d;
}

void shader::reload() {
  LOG(INFO) << "reloading shaders (or rather, should be...)";
}

void compile_info(const GLuint shader) {
  GLint status(0);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  GLint maxLength(0);
  GLint logLength(0);

  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

  GLchar *log = new GLchar[maxLength];
  glGetShaderInfoLog(shader, maxLength, &logLength, log);

  if (1 != status) {
    LOG(ERROR) << "glsl compile error: " << log;
  } else if (maxLength) {
    LOG(INFO) << log;
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

    LOG(ERROR) << "glsl link error: " << log;
  }
}

void load_shader(const GLuint shader, std::string fn) {
  std::ifstream srcfile(fn, std::ios::in | std::ios::binary);

  if (!srcfile.is_open()) {
    LOG(FATAL) << "couldn't open: " << fn;
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
  GLint max_ufrm_namelength = 0;

  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &n_active_attr);
  glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &n_active_ufrm);

  glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attr_namelength);
  glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_ufrm_namelength);

  std::vector<GLchar> nameData(max_attr_namelength);
  for (int attr = 0; attr < n_active_attr; ++attr) {
    GLint arraySize = 0;
    GLenum type = GL_FLOAT;
    GLsizei actualLength = 0;
    glGetActiveAttrib(program, attr, nameData.size(), &actualLength, &arraySize, &type, &nameData[0]);

    LOG(INFO) << "attr " << attr << ": " << std::string((char *)&nameData[0], actualLength);
  }

  nameData = std::vector<GLchar>(max_ufrm_namelength);
  for (int ufrm = 0; ufrm < n_active_ufrm; ++ufrm) {
    GLint arraySize = 0;
    GLenum type = GL_FLOAT;
    GLsizei actualLength = 0;
    glGetActiveUniform(program, ufrm, nameData.size(), &actualLength, &arraySize, &type, &nameData[0]);
    LOG(INFO) << "ufrm " << ufrm << ": " << std::string((char *)&nameData[0], actualLength);
  }
}
