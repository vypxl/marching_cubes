#version 300 es
#extension GL_EXT_geometry_shader : enable
#extension GL_OES_geometry_shader : enable
#extension GL_EXT_gpu_shader4 : enable

precision mediump float;

layout(points) in;
layout(triangle_strip, max_vertices = 12) out;

in vec3 _pos[];
out vec3 pos;

uniform mat4 MVP;
uniform isampler2D triTable;
uniform float surface;

const vec3 cube[8] = vec3[8](
    vec3(0.0, 0.0, 1.0),
    vec3(1.0, 0.0, 1.0),
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 0.0, 0.0),
    vec3(0.0, 1.0, 1.0),
    vec3(1.0, 1.0, 1.0),
    vec3(1.0, 1.0, 0.0),
    vec3(0.0, 1.0, 0.0)
);

int getTriangulation(int i, int j) {
    return texelFetch2D(triTable, ivec2(j, i), 0).r;
}

float f(vec3 position) {
    const float resolution = 4.0;
    // sum of elements
    /* return dot(position * vec3(resolution * resolution, resolution, 1), vec3(1)); */
    if ((position.x > 1.0) && (position.y > 1.0) && (position.z > 1.0)) {
      return 5.0;
    } else {
      return -5.0;
    }
}

vec3 middle(vec3 a, vec3 b) {
    return (a + b) / 2.0;
}

int whichCase(vec3 position) {
    int idx = 0;

    idx |= int(f(position + cube[0]) < surface) << 0;
    idx |= int(f(position + cube[1]) < surface) << 1;
    idx |= int(f(position + cube[2]) < surface) << 2;
    idx |= int(f(position + cube[3]) < surface) << 3;
    idx |= int(f(position + cube[4]) < surface) << 4; 
    idx |= int(f(position + cube[5]) < surface) << 5; 
    idx |= int(f(position + cube[6]) < surface) << 6; 
    idx |= int(f(position + cube[7]) < surface) << 7;  

    return idx;
}

void main() {
    vec3 _pos = _pos[0];

    int c = whichCase(_pos);
    if (c == 0 || c == 255) return;
    vec3 verts[12];

    #define mkCase(i, ci, cj) verts[i] = _pos + middle(cube[ci], cube[cj]);

    mkCase(0,  0, 1)
    mkCase(1,  1, 2)
    mkCase(2,  2, 3)
    mkCase(3,  3, 0)
    mkCase(4,  4, 5)
    mkCase(5,  5, 6)
    mkCase(6,  6, 7)
    mkCase(7,  7, 4)
    mkCase(8,  0, 4)
    mkCase(9,  1, 5)
    mkCase(10, 2, 6)
    mkCase(11, 3, 7)

    for (int i = 0; i < 16; i += 3) {
      if (getTriangulation(c, i) == -1) continue;

      for (int j = 0; j < 3; j++) {
        pos = verts[getTriangulation(c, i + j)];
        gl_Position = MVP * vec4(pos, 1.0);
        EmitVertex();
      }

      EndPrimitive();
    }
}

