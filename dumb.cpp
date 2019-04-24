#include <glfw_app.hpp>

int main(int argc, char** argv) {
    glfw_app gltest([](){}, [](){});
    gltest.run();
    return 0;
}
