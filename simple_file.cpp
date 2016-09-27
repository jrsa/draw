#include "simple_file.hpp"

#include <fstream>
#include <glog/logging.h>

std::string simple_file::read(std::string fn) {
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
  std::string contents(buf);
  delete[] buf;

  return contents;
}