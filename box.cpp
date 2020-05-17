#include <gl_shared.hpp>
#include <glfw_app.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glog/logging.h>

#include <iostream>

#include <shader.hpp>

using namespace gl;
using namespace glm;

shader* draw = nullptr;

int w = 800, h = 600;

GLfloat vertices[] = {
    -1.f, -1.f, -1.f,
    -1.f, -1.f,  1.f,
    -1.f,  1.f, -1.f,
    -1.f,  1.f,  1.f,
     1.f, -1.f, -1.f,
     1.f, -1.f,  1.f,
     1.f,  1.f, -1.f,
     1.f,  1.f,  1.f 
};

GLuint elements[] = {
    2, 3, 6,
    6, 3, 7,
    0, 1, 2,
    2, 1, 3,
    1, 5, 3, 
    5, 7, 3,
    5, 4, 7,
    4, 6, 7,
    4, 0, 6,
    0, 2, 6,
    5, 1, 4,
    1, 0, 4
};

void load_shaders() {
    draw = new shader("mesh/basic3d", "mesh/hsv3d");
}

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
    GLuint vao, vbo, ebo;

    auto setup_proc = [&] {
        glEnable(GL_DEPTH_TEST); // always

        load_shaders();
        draw->use();

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

        glClearColor(1.0, 1.0, 1.0, 1.0);
    };

    auto draw_proc = [&] {
        float t = glfwGetTime();

        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        draw->use();

        mat4 model (1.0);
        model = rotate(model, t * radians(10.0f), vec3(0.0f, 1.0f, 0.0f));
        model = scale(model, vec3(0.5, 0.5, 0.5));
        mat4 view = lookAt(vec3(1.2f, 1.2f, 1.2f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
        mat4 proj = perspective(radians(55.0f), (float) w / (float) h, 0.10f, 10.0f);

        draw->u44m("model", model);
        draw->u44m("view", view);
        draw->u44m("proj", proj);
        glDrawElementsInstanced(GL_TRIANGLES, sizeof(elements) / sizeof(GLuint), GL_UNSIGNED_INT, 0, 1);
    };

    glfw_app gltest(draw_proc, setup_proc);

    gltest.set_key_proc([](GLFWwindow* window, int k, int, int a, int) {
        if (a == GLFW_PRESS) {
            switch (k) {
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

    gltest.set_cursor_proc([](GLFWwindow*, double x, double y) {
        float xnorm = x / w;
        float ynorm = y / h;
        draw->u2f("mouse", glm::vec2(xnorm, ynorm));
    });

    gltest.set_fbsize_proc([](GLFWwindow* window, int width, int height) {
        w = width;
        h = height;
        glViewport(0, 0, width, height);
    });

    gltest.run();
    return 0;
}
