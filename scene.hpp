#ifndef _scene_hpp
#define _scene_hpp


#include <string>
#include <utility>


struct scene {
  std::pair<std::string, std::string> src;
  std::pair<std::string, std::string> front;
  std::pair<std::string, std::string> back;
  std::pair<std::string, std::string> post;
};

#endif