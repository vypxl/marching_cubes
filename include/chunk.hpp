#ifndef __chunk_hpp_
#define __chunk_hpp_

#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "camera.hpp"
#include "shader.hpp"

class Chunk {
    // must be a cubic
    static const int points_count = 32 * 32 * 32;

    GLuint vao, wireframe_vao, vbo;

public:
    Chunk(glm::vec3 position, GLuint mcubes_attr_loc, GLuint wireframe_attr_loc);
    ~Chunk();

    void render(Camera *cam, Shader *mcubesShader, Shader *wireframeShader, bool drawWireframe);
};

class ChunkManager {
    bool drawWireframe = false;

    float surface = 0.f;
    const int num_funcs = 6;
    int func = 0;

    GLuint triTableTex;

    Shader mcubesShader;
    Shader wireframeShader;

    std::vector<std::unique_ptr<Chunk>> chunks;

public:
    void init();
    ~ChunkManager();

    void render(Camera *cam);
    void reloadShaders();
    inline void setSurface(float surface) { this->surface = surface; }
    inline void nextFunc(int dt = 1) { func = ((func + dt) % num_funcs + num_funcs) % num_funcs; }
};

#endif

