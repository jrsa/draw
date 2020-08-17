#include <glbinding/Binding.h>
#include <glog/logging.h>

#include "billboard.hpp"
#include "gl_shared.hpp"
#include "glfw_app.hpp"
#include "shader.hpp"

billboard* quad = nullptr;
shader* s = nullptr;
std::string filename, vs_filename;

int h = 0, w = 0;

int main(int argc, const char* argv[])
{
    shader::setdir("/Users/jrsa/code/gl/glsl/img/");

    if (argc >= 2) {
        filename = argv[1];
    } else {
        LOG(FATAL) << "specify fragment shader name as argument";
    }

    if (argc >= 3) {
        vs_filename = argv[2];
    } else {
        vs_filename = "passthru";
    }

    auto setup_proc = [&] {
        quad = new billboard(); // constructor performs openGL initialization
        s = new shader(vs_filename, filename);
    };
    auto draw_proc = [&] {
        s->use();
        quad->draw();
    };

    glfw_app gltest(draw_proc, setup_proc);

    gltest.set_key_proc([](int k, int, int a, int) {
        if (a == GLFW_PRESS) {
            if (k >= 48 && k <= 57) {
                int code = k - 48;
                s->u1f("hue", k / 10.0);
            }
            switch (k) {
            case 'R': {
                s = new shader(vs_filename, filename);
                break;
            }
            default: {
                break;
            }
            }
        }
    });

    gltest.set_fbsize_proc([](int width, int height) {
        glViewport(0, 0, width, height);
    });

    gltest.run();
    return 0;
}
