#include <gl_shared.hpp>
#include <glfw_app.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glog/logging.h>

#include <shader.hpp>
#include <box.hpp>

using namespace gl;
using namespace glm;

int main(int argc, char** argv) {
    const char* shader_path = getenv("SHADER_PATH");

    if (shader_path)
    {
        shader::setdir(shader_path);
    }
    else
    {
        LOG(FATAL) << "missing SHADER_PATH environment variable";
    }

    glfw_app gltest;

    {
        // gl objects -------------------------------------------------------------------- 
        glEnable(GL_DEPTH_TEST); // always

        box b;

        shader draw ("mesh/basic3d", "mesh/hsv3d");
        draw.use();
        b.init();

        int w = 0, h = 0;

        glClearColor(1.0, 1.0, 1.0, 1.0);

        // callbacks --------------------------------------------------------------------- 
        gltest.set_fbsize_proc([&w, &h](int width, int height) {
            w = width;
            h = height;
            glViewport(0, 0, width, height);
        });

        gltest.set_cursor_proc([&](double x, double y) {
            float xnorm = x / w;
            float ynorm = y / h;
            draw.u2f("mouse", glm::vec2(xnorm, ynorm));
        });

        gltest.onkey('R', [&draw] () { draw.reload(); });

        // run, with "frame" callback ---------------------------------------------------- 
        gltest.run([&] {
            glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

            draw.use();

            mat4 model (1.0);
            model = rotate(model, (float)glfwGetTime() * radians(10.0f), vec3(0.0f, 1.0f, 0.0f));
            model = scale(model, vec3(0.5, 0.5, 0.5));
            mat4 view = lookAt(vec3(1.2f, 1.2f, 1.2f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
            mat4 proj = perspective(radians(55.0f), (float) w / (float) h, 0.10f, 10.0f);

            draw.use();

            draw.u44m("model", model);
            draw.u44m("view", view);
            draw.u44m("proj", proj);

            draw.u1f("time", glfwGetTime());

            b.draw();
        });
    }

    return 0;
}
