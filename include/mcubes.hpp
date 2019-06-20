#ifndef __mcubes_hpp_
#define __mcubes_hpp_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glapp.hpp"
#include "camera.hpp"
#include "chunk.hpp"

class MCubes : public GlApp {

    float camSpeed = 5.0;
    float camRX = 0;
    float camRY = 0;

    SDL_GLContext glcontext;
    SDL_Window *window;

    FtFont* font;

    ChunkManager chunkManager;

    Camera cam;

    Uint32 last_timer = 0;
    double delta = 0;
    bool mouseCaptured = false;

    float surface = 0;

    void update();
    void draw() ;
    void cleanup();

public:
    int init();
    void mainloop();
};

#endif
