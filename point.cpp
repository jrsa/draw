#include <gl_shared.hpp>
#include <glfw_app.hpp>

#include <billboard.hpp>
#include <fbo.h>
#include <shader.hpp>

using namespace gl;
using namespace glm;

vec3 point[] = { { 0.0f, 0.0f, 0.0f } };

fbo* accum = nullptr;
billboard* bb = nullptr;
shader* slab_pass = nullptr;

shader* displace = nullptr;

bool clear = true;

// output framebuffer size
int w, h;

void load_shaders()
{
    slab_pass = new shader("img/passthru");
    displace = new shader("mesh/basic");
}

int main(int argc, char** argv)
{
    shader::setdir("/Users/jrsa/code/gl/glsl/");
    GLuint vao, vbo;

    auto setup_proc = [&] {
        // all this to draw one point at [ 0, 0 ]
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // generate and upload to vbo
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(point), point, GL_STATIC_DRAW);

        // bind the current vbo to attribute location 0 in shader (?)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // unbind vao
        glBindVertexArray(0);

        load_shaders();
        displace->use();

        bb = new billboard();
        accum = new fbo(2000, 2000);

        glPointSize(5.0);
    };

    auto draw_proc = [&] {
        // if (clear) {
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        // }

        displace->u1f("time", glfwGetTime());

        accum->bind();
        glViewport(0, 0, 2000, 2000);
        if (clear) {
            glClear(GL_COLOR_BUFFER_BIT);
        }
        
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glDrawArrays(GL_POINTS, 0, 2);

        fbo::unbind_all();

        glViewport(0, 0, w, h);
        accum->bind_tex();
        slab_pass->use();
        bb->draw();
    };

    glfw_app gltest(draw_proc, setup_proc);

    gltest.set_key_proc([](int k, int, int a, int) {
        if (a == GLFW_PRESS) {
            switch (k) {
            case 'C': {
                clear = !clear;
                break;
            }
            case 'R': {
                load_shaders();
                break;
            }
            default: {
                break;
            }
            }
        }
    });

    gltest.set_cursor_proc([&](double x, double y) {
        displace->u2f("mouse", glm::vec2(x / h, y / w));
    });

    gltest.set_fbsize_proc([](int width, int height) {
        // these only get saved so that the viewport can be restored
        // after drawing FBOs
        w = width;
        h = height;
        glViewport(0, 0, width, height);
    });

    gltest.run();
    return 0;
}
