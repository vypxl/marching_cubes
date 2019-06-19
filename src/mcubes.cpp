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

    // process input
    const Uint8 *keyState = SDL_GetKeyboardState(nullptr);
    glm::vec3 movement(0.f, 0.f, 0.f);

    if (keyState[SDL_SCANCODE_S])      movement += glm::vec3(-camSpeed * delta * cos(camRX), 0.f, -camSpeed * delta * sin(camRX));
    if (keyState[SDL_SCANCODE_W])      movement += glm::vec3( camSpeed * delta * cos(camRX), 0.f,  camSpeed * delta * sin(camRX));
    if (keyState[SDL_SCANCODE_SPACE])  movement += glm::vec3(0.f, camSpeed * delta, 0.f);
    if (keyState[SDL_SCANCODE_LSHIFT]) movement += glm::vec3(0.f, -camSpeed * delta, 0.f);
    if (keyState[SDL_SCANCODE_A])      movement += glm::vec3(-camSpeed * delta * cos(camRX + M_PI_2), 0.f, -camSpeed * delta * sin(camRX + M_PI_2));
    if (keyState[SDL_SCANCODE_D])      movement += glm::vec3(-camSpeed * delta * cos(camRX - M_PI_2), 0.f, -camSpeed * delta * sin(camRX - M_PI_2));
    if (keyState[SDL_SCANCODE_LCTRL])  movement *= 3.f;

    cam.move(movement);

    // update camera rotation
    int mouseX, mouseY;
    SDL_GetRelativeMouseState(&mouseX, &mouseY);

    const float mouseSensivity = 0.1f;
    camRX += glm::radians(mouseX * mouseSensivity);
    camRY -= glm::radians(mouseY * mouseSensivity);
    const float max_angle = 1.57079;
    if (camRY > max_angle)  camRY = max_angle;
    if (camRY < -max_angle) camRY = -max_angle;

    cam.setRotation(camRX, camRY);
    std::cout << glm::degrees(camRX) << " | " << glm::degrees(camRY) << std::endl;
}

void MCubes::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw status text
    char buf[128];
    snprintf(buf, sizeof(buf), "Marching Cubes | surface: %f", surface);
    font->renderText(buf, 10, height - 26, 1.f);

    // draw marching cubes
    const int distance = 15;
    glm::mat4 mMVP = cam.getMVP();

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

    SDL_GL_SwapWindow(window);
}

void MCubes::cleanup() {
    glDeleteTextures(1, &triTableTex);
    delete font;
}

int MCubes::init() {
    // Initialize SDL
    int width = 1920;
    int height = 1080;
    window = SDL_CreateWindow("Marching cubes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MOUSE_CAPTURE);
    glcontext = SDL_GL_CreateContext(window);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

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
    cam.moveTo(glm::vec3(15.f, 5.f, 15.f));
    cam.moveTo(glm::vec3(0.5f,0.5f,0.5f));

    // set up attribute locations
    mcubes_attr_pos = mCubesShader.getAttribLocation("position");
    cube_attr_pos = cubeShader.getAttribLocation("position");

    // set up vaos
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
    update();

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_q:
                    running = false;
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
        case SDL_WINDOWEVENT_RESIZED:
            // set instance members
            width = event.window.data1;
            height = event.window.data2;

            // set gl viewport
            glViewport(0, 0, width, height);

            // set font projection
            font->setWindow(width, height);
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_RIGHT) {
              mouseCaptured = !mouseCaptured;
              SDL_SetRelativeMouseMode((SDL_bool)mouseCaptured);
            }
        }
    }

    draw();
}
