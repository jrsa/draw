test bed for simple opengl rendering tasks.

there are 3 standalone executables in the repository.
  * fbloop is a video feedback loop implemented using glsl shaders.
  * xformfb is a particle system using an array of points processed using transform feedback
  * point is a basic openGL example that draws a single point

depends on:
  * glfw3
  * glbinding
  * glog
  * glm

to build:

    cd <repo>
    mkdir build
    cd build
    cmake ..
    make
