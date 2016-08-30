//
// Created by James Anderson on 8/29/16.
//

#ifndef PROJECT_WATCHER_H
#define PROJECT_WATCHER_H

#include <CoreServices/CoreServices.h>
#include <string>

class watcher {
public:
  watcher(std::string dir);

private:
  CFStringRef _path;
};


#endif //PROJECT_WATCHER_H
