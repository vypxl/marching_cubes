#include <iostream>
#include <assert.h>
#include <cmath>

#include "tritable.hpp"
#include "mcubes.hpp"

void MCubes::update() {
    // update delta time
    delta = (double)((SDL_GetTicks() - last_timer)) / 1000.f;
    last_timer = SDL_GetTicks();

    // update view angle
    angle = std::fmod(angle + ((M_PI / 3) * delta), M_PI * 2);

    // update surface level according to mouse position on screen
    int mouseY;
    SDL_GetMouseState(nullptr, &mouseY);
    const double range = 20;
    surface = -((((double) mouseY) / ((double) height)) * range - (range / 2.0));
}

void MCubes::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO fix flickering
    // TODO controllable camera

    // draw status text
    char buf[128];
    snprintf(buf, sizeof(buf), "Marching Cubes | surface: %f", surface);
    font->renderText(buf, 10, height - 26, 1.f);

    // draw marching cubes
    const int distance = 15;
    cam.moveTo(glm::vec3(sin(angle) * distance, distance * 3 / 5, cos(angle) * distance));
    cam.lookAt(glm::vec3(0, 0, 0));
    glm::mat4 mMVP = cam.getMVP(); // mProjection * mView * mModel;

    mCubesShader.bind();

    mCubesShader.setUniform("MVP", mMVP);
    mCubesShader.setUniform("triTable", 0);
    mCubesShader.setUniform("surface", surface);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, triTableTex);

    glBindVertexArray(mcubes_vao);
    glDrawArrays(GL_POINTS, 0, points_count);

    // draw box outlines if enabled
    if (drawBoxOutline) {
      cubeShader.bind();
      cubeShader.setUniform("MVP", mMVP);

      glBindVertexArray(cube_vao);
      glDrawArrays(GL_POINTS, 0, points_count);
    }

    glBindVertexArray(0);

    SDL_GL_SwapBuffers();
}

void MCubes::cleanup() {
    glDeleteTextures(1, &triTableTex);
    delete font;
}

int MCubes::init() {
    // Initialize SDL
    const SDL_VideoInfo* info = nullptr;
    int width = 1920;
    int height = 1080;

    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Video initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    info = SDL_GetVideoInfo( );
    if(!info) {
        std::cerr << "Video query failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    if(SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_NOFRAME) == 0) {
        std::cerr << "Video mode set failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize glew
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Initialize OpenGL
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 0.f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* glEnable(GL_CULL_FACE); */
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_TEXTURE_2D);

    // Initialize font
    font = new FtFont("res/RobotoMono-Regular.ttf", 16, width, height);

    // Initialize shaders
    mCubesShader.loadFromFile("shaders/mcubes.vert", "shaders/mcubes.geom", "shaders/mcubes.frag");
    cubeShader.loadFromFile("shaders/cubes.vert", "shaders/cubes.geom", "shaders/cubes.frag");

    // Initialize Buffers
    glGenVertexArrays(1, &mcubes_vao);
    glGenVertexArrays(1, &cube_vao);
    glGenBuffers(1, &points_vbo);

    // Initialize points
    const int limit = std::cbrt(points_count);
    for (float i = 0; i < limit; i++)
      for (float j = 0; j < limit; j++)
        for (float k = 0; k < limit; k++) {
          const int idx = i * limit * limit + j * limit + k;
          points[idx][0] = i;
          points[idx][1] = j;
          points[idx][2] = k;
        }

    glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Set up triTable texture
    glGenTextures(1, &triTableTex);
    glBindTexture(GL_TEXTURE_2D, triTableTex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8I, 15, 256, 0, GL_RED_INTEGER, GL_BYTE, (GLint*)triTable);
    glBindTexture(GL_TEXTURE_2D, 0);

    // set up camera
    cam.setProjection(45, width, height);

    mcubes_attr_pos = mCubesShader.getAttribLocation("position");
    cube_attr_pos = cubeShader.getAttribLocation("position");

    glBindVertexArray(mcubes_vao);

      glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
      glVertexAttribPointer(mcubes_attr_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
      glEnableVertexAttribArray(mcubes_attr_pos);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    glDisableVertexAttribArray(mcubes_attr_pos);

    glBindVertexArray(cube_vao);

      glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
      glVertexAttribPointer(cube_attr_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
      glEnableVertexAttribArray(cube_attr_pos);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    glDisableVertexAttribArray(cube_attr_pos);

    return 0;
}

void MCubes::mainloop() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_q:
                    running = false;
                    break;
                case SDLK_UP:
                    surface += 0.5f;
                    break;
                case SDLK_DOWN:
                    surface -= 0.5f;
                    break;
                case SDLK_r:
                    mCubesShader.loadFromFile("shaders/mcubes.vert", "shaders/mcubes.geom", "shaders/mcubes.frag");
                    break;
                default:
                    break;
            }
            break;
        case SDL_QUIT:
            running = false;
            break;
        case SDL_VIDEORESIZE:
            // set instance members
            width = event.resize.w;
            height = event.resize.h;

            // set sdl videomode
            if(SDL_SetVideoMode(width, height, 0, SDL_OPENGL | SDL_NOFRAME) == 0) {
                std::cerr << "Video mode set failed: " << SDL_GetError() << std::endl;
                exit(1);
            }

            // set gl viewport
            glViewport(0, 0, width, height);

            // set font projection
            font->setWindow(width, height);
            break;
        }
    }

    update();
    draw();
}
