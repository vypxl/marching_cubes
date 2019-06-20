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
    /* angle = std::fmod(angle + ((M_PI / 3) * delta), M_PI * 2); */

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
    if (mouseCaptured) {
        const float mouseSensivity = 0.1f;
        camRX += glm::radians(mouseX * mouseSensivity);
        camRY -= glm::radians(mouseY * mouseSensivity);
        const float max_angle = 1.57079;
        if (camRY > max_angle)  camRY = max_angle;
        if (camRY < -max_angle) camRY = -max_angle;

        cam.setRotation(camRX, camRY);
    }
}

void MCubes::draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw status text
    char buf[128];
    snprintf(buf, sizeof(buf), "Marching Cubes | surface: %f", surface);
    font->renderText(buf, 10, height - 26, 1.f);

    // draw marching cubes
    glm::mat4 mMVP = cam.getMVP();
    chunkManager.render(&cam);

    SDL_GL_SwapWindow(window);
}

void MCubes::cleanup() {
    delete font;
}

int MCubes::init() {
    // Initialize SDL
    int width = 1920;
    int height = 1080;
    window = SDL_CreateWindow(
        "Marching cubes",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_MOUSE_CAPTURE
      );
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

    // set up camera
    cam.setProjection(45, width, height);
    cam.moveTo(glm::vec3(-1.f, 2.f, -1.f));
    cam.lookAt(glm::vec3(1.f, 0.f, 1.f));

    chunkManager.init();
    chunkManager.setSurface(surface);

    return 0;
}

void MCubes::mainloop() {
    update();

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
        case SDL_MOUSEWHEEL:
            surface += event.wheel.y;
            surface += event.wheel.x * 100;
            chunkManager.setSurface(surface);
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_q:
                    running = false;
                    break;
                case SDLK_r:
                    chunkManager.reloadShaders();
                    break;
                case SDLK_UP:
                    surface++;
                    chunkManager.setSurface(surface);
                    break;
                case SDLK_DOWN:
                    surface--;
                    chunkManager.setSurface(surface);
                    break;
                case SDLK_LEFT:
                    chunkManager.nextFunc(-1);
                    break;
                case SDLK_RIGHT:
                    chunkManager.nextFunc();
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
