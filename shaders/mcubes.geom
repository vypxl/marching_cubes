#version 300 es
#extension GL_EXT_geometry_shader : enable
#extension GL_OES_geometry_shader : enable
#extension GL_EXT_gpu_shader4 : enable

precision highp float;

layout(points) in;
layout(triangle_strip, max_vertices = 12) out;

in vec3 _pos[];
out vec3 pos;

uniform mat4 MVP;
uniform isampler2D triTable;
uniform float surface;
uniform int func;

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
    vec3 p = position;

    switch (func) {
      case 0: // sphere
        return (p.x * p.x + p.y * p.y + p.z * p.z) - 4096.0;
        break;
      case 1: // plane
        return 3.0 * p.x + 4.0 * p.y - p.z - 5.0;
        break;
      case 2: // ripple
        p *= 0.3;
        return sin(length(p)) * p.x + sin(length(p)) * p.y;
        break;
      case 3: // hyperboloid
        return (pow(p.x, 2.0) + pow(p.y, 2.0) - pow(p.z, 2.0)) * 0.025;
        break;
      case 4: // hyperbolic parabloid
        return (pow(p.x, 2.0) - pow(p.y, 2.0) - p.z) * 0.025;
        break;
      case 5: // crazy laggy 
        p *= 100.0;
        return (sin(dot(p.xxy, p.yzz)) + sin(p.x * p.y) + sin(p.x * p.z)) * 1.0;
        break;
      default:
        break;
    }
}

vec3 middle(vec3 a, vec3 b) {
    float fa = f(a);
    float fb = f(b);
    if (abs(fa - fb) < 0.00001 || abs(surface - fa) < 0.00001) return a;
    if (abs(surface - fb) < 0.00001) return b;
    return mix(a, b, (surface - fa) / (fb - fa));
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

    #define mkCase(i, ci, cj) verts[i] = middle(_pos + cube[ci], _pos + cube[cj]);

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

