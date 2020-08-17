#ifndef _box_hpp_
#define _box_hpp_

#include <gl_shared.hpp>

class box {
    // indexed mesh
    GLfloat vertices_[24] = {
        -1.f, -1.f, -1.f,
        -1.f, -1.f,  1.f,
        -1.f,  1.f, -1.f,
        -1.f,  1.f,  1.f,
         1.f, -1.f, -1.f,
         1.f, -1.f,  1.f,
         1.f,  1.f, -1.f,
         1.f,  1.f,  1.f 
    };
    GLuint elements_[36] = {
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


    GLuint vao = 0, vbo = 0, ebo = 0;
public:
    void init() {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_), vertices_, GL_STATIC_DRAW);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements_), elements_, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
        glBindVertexArray(0);
    }

    void draw() {

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(elements_) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
    }
};

#endif // _box_hpp_
