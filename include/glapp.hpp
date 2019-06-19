#ifndef __viz_hpp_
#define __viz_hpp_

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include <GL/glew.h>
#endif
#include <SDL2/SDL.h>

#include "shader.hpp"
#include "ftfont.hpp"

class GlApp {
protected:
    bool running = false;
    int width = 1920;
    int height = 1080;

    virtual void update() {};
    virtual void draw() {};
    virtual void cleanup() {};

public:
    /// initialize the display
    virtual int init() { return 0; };
    virtual void mainloop() {};
    void loop();
};

#endif
