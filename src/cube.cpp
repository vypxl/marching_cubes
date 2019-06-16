#include "cube.hpp" 
float f(float x, float y, float z) {
    const int resolution = 4;
    return z * resolution * resolution + y * resolution + x;
}

void get_middle(float* v1, float* v2, float* target) {
    target[0] = (v1[0] + v2[0]) / 2.f;
    target[1] = (v1[1] + v2[1]) / 2.f;
    target[2] = (v1[2] + v2[2]) / 2.f;
}

void render_cube() {
    // float cube[8][3] {
    //     {1.f, 0.f, 1.f},
    //     {0.f, 0.f, 1.f},
    //     {0.f, 0.f, 0.f},
    //     {1.f, 0.f, 0.f},
    //     {1.f, 1.f, 1.f},
    //     {0.f, 1.f, 1.f},
    //     {0.f, 1.f, 0.f},
    //     {1.f, 1.f, 0.f}
    // };

    // // translate the cube
    // float translate[3] = {-0.5f, 0.0f, -0.5f};
    // // float translate[3] = {0.0f, 0.0f, 0.0f};
    // for(int i = 0; i <= 8; i++) {
    //     cube[i][0] += translate[0];
    //     cube[i][1] += translate[1];
    //     cube[i][2] += translate[2];
    // }

    // // compute the triangulation
    // uint index = 0;
    // for(int i = 0; i < 8; i++) {
    //     bool above_surface = f(cube[i][0], cube[i][1], cube[i][2]) < surface;
    //     index |= (above_surface ? 1 : 0) << i;
    // }
    // assert(index <= 255);
    // const char* triangulation = triTable[index];

    // float vertices[12][3];
    // int vcount = 0;

    // for (int i = 0; i < 12; i++) {
    //     if (triangulation[i] == -1) break;
    //     vcount++;

    //     // Here maybe add interpolation
    //     const char* verts = edgeTable[triangulation[i]];
    //     get_middle(cube[verts[0]], cube[verts[1]], vertices[i]);
    // }
    
    // assert(vcount % 3 == 0);

    // // Draw the cube
    // shader.bind();

    // glm::mat4 projection = glm::perspective(glm::radians(45.f), (float) width / (float) height, 0.1f, 100.f);
    // glm::mat4 view = glm::lookAt(glm::vec3(sin(angle) * 5, 3, cos(angle) * 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    // glm::mat4 model = glm::mat4(1.f);
    // glm::mat4 mvp = projection * view * model;
    // GLuint mvp_loc = glGetUniformLocation(shader.getNativeHandle(), "MVP");
    // glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &mvp[0][0]);

    // GLuint wfr_loc = glGetUniformLocation(shader.getNativeHandle(), "in_wireframe");

    // glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // // Draw marched cube
    // glBufferData(GL_ARRAY_BUFFER, vcount * 3 * sizeof(float), vertices, GL_STREAM_DRAW);
    // glVertexAttribPointer(attr_pos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    // glEnableVertexAttribArray(attr_pos);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_ebo);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glUniform1i(wfr_loc, 0);
    // glDrawArrays(GL_TRIANGLES, 0, vcount);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glUniform1i(wfr_loc, 1);
    // glDrawArrays(GL_TRIANGLES, 0, vcount);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Draw cube
    // glBufferData(GL_ARRAY_BUFFER, 8 * 3 * sizeof(float), cube, GL_STREAM_DRAW);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // glUniform1i(wfr_loc, 0);
    // glDrawElements(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_INT, nullptr);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glUniform1i(wfr_loc, 1);
    // glDrawElements(GL_TRIANGLES, 6 * 2 * 3, GL_UNSIGNED_INT, nullptr);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


    // glDisableVertexAttribArray(attr_pos);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
}
