#include "chunk.hpp"
#include "tritable.hpp"

Chunk::Chunk(glm::vec3 position, GLuint mcubes_attr_loc, GLuint wireframe_attr_loc) {
    // Initialize Buffers
    glGenVertexArrays(1, &vao);
    glGenVertexArrays(1, &wireframe_vao);
    glGenBuffers(1, &vbo);

    // Initialize points
    float points[points_count][3];
    const int limit = std::cbrt(points_count);
    for (float i = 0; i < limit; i++)
      for (float j = 0; j < limit; j++)
        for (float k = 0; k < limit; k++) {
          const int idx = i * limit * limit + j * limit + k;
          points[idx][0] = position.x + i;
          points[idx][1] = position.y + j;
          points[idx][2] = position.z + k;
        }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set up vaos
    glBindVertexArray(vao);

      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glVertexAttribPointer(mcubes_attr_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
      glEnableVertexAttribArray(mcubes_attr_loc);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    glDisableVertexAttribArray(mcubes_attr_loc);

    glBindVertexArray(wireframe_vao);

      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glVertexAttribPointer(wireframe_attr_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
      glEnableVertexAttribArray(wireframe_attr_loc);
      glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
    glDisableVertexAttribArray(wireframe_attr_loc);
}

Chunk::~Chunk() {
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &wireframe_vao);
    glDeleteBuffers(1, &vbo);
}

void Chunk::render(Camera *cam, Shader *mcubesShader, Shader *wireframeShader, bool drawWireframe) {
    mcubesShader->bind();

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, points_count);

    // draw box outlines if enabled
    if (drawWireframe) {
      wireframeShader->bind();

      glBindVertexArray(wireframe_vao);
      glDrawArrays(GL_POINTS, 0, points_count);
    }

    glBindVertexArray(0);
}


void ChunkManager::init() {
    // set up shaders
    reloadShaders();

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

    // creating chunks
    GLuint mcubes_attr_loc = mcubesShader.getAttribLocation("position");
    GLuint wireframe_attr_loc = wireframeShader.getAttribLocation("position");

    const int start = -4;
    const int end = 4;
    chunks.reserve((end - start) * (end - start) * (end - start));
    for (int x = start; x < end; x++)
      for (int y = start; y < end; y++)
        for (int z = start; z < end; z++)
          chunks.emplace_back(std::make_unique<Chunk>(
            glm::vec3(32 * x, 32 * y, 32 * z), mcubes_attr_loc, wireframe_attr_loc
          ));
}

ChunkManager::~ChunkManager() {
    glDeleteTextures(1, &triTableTex);
}

void ChunkManager::render(Camera *cam) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, triTableTex);

    cam->setModel(glm::scale(glm::mat4(1.f), glm::vec3(1.f / 32.f)));
    glm::mat4 mMVP = cam->getMVP();
    cam->setModel(glm::mat4(1.f));

    mcubesShader.bind();
    mcubesShader.setUniform("surface", surface);
    mcubesShader.setUniform("func", func);
    mcubesShader.setUniform("MVP", mMVP);
    mcubesShader.setUniform("triTable", 0);

    if (drawWireframe) {
      wireframeShader.bind();
      wireframeShader.setUniform("MVP", mMVP);
    }

    for (auto& c : chunks) {
      c->render(cam, &mcubesShader, &wireframeShader, drawWireframe);
    }
}

void ChunkManager::reloadShaders() {
    mcubesShader.loadFromFile("shaders/mcubes.vert", "shaders/mcubes.geom", "shaders/mcubes.frag");
    wireframeShader.loadFromFile("shaders/wireframe.vert", "shaders/wireframe.geom", "shaders/wireframe.frag");
}

// TODO
// dynamic chunk loading
// send voxel data to gpu
// precompute triangulation via compute shader and only render the final mesh
// share vertices via ebo

