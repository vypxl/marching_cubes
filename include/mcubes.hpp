#ifndef __mcubes_hpp_
#define __mcubes_hpp_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glapp.hpp"
#include "camera.hpp"

class MCubes : public GlApp {

    float camSpeed = 5.0;
    float camRX = 0;
    float camRY = 0;

    SDL_GLContext glcontext;
    SDL_Window *window;

    Shader mCubesShader;
    Shader cubeShader;
    FtFont* font;

    GLuint mcubes_attr_pos;
    GLuint cube_attr_pos;

    GLuint triTableTex;

    GLuint mcubes_vao;
    GLuint cube_vao;
    GLuint points_vbo;
    GLuint cube_ebo;

    Camera cam;

    Uint32 last_timer = 0;
    double delta = 0;
    bool mouseCaptured = false;

    float surface = 3;
    float angle = 0;

    bool drawBoxOutline = true;

    static const uint points_count = 125;
    float points[points_count][3];

    void update();
    void draw() ;
    void cleanup();

public:
    int init();
    void mainloop();
};

#endif
