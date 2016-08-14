#ifndef fbo_h
#define fbo_h

class fbo {

public:
    fbo(int, int);
    ~fbo();

    void bind();
    void bind_tex();
    bool is_complete();

    static void unbind_all();

private:
    unsigned int _fbo_obj;
    unsigned int _colorbuffer_texture_obj;
};


#endif //PROJECT_FBO_H
