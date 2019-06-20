#version 300 es
#extension GL_EXT_geometry_shader : enable
#extension GL_OES_geometry_shader : enable
#extension GL_EXT_gpu_shader4 : enable

precision mediump float;

layout(points) in;
layout(line_strip, max_vertices = 17) out;

in vec3 _pos[];

out vec3 pos;

uniform mat4 MVP;

vec3 cube[8] = vec3[8] (
  vec3(0.0, 0.0, 0.0),
  vec3(0.0, 0.0, 1.0),
  vec3(0.0, 1.0, 0.0),
  vec3(0.0, 1.0, 1.0),
  vec3(1.0, 0.0, 0.0),
  vec3(1.0, 0.0, 1.0),
  vec3(1.0, 1.0, 0.0),
  vec3(1.0, 1.0, 1.0)
);

#define emit(idx) gl_Position = MVP * vec4(pos + cube[idx], 1.0); EmitVertex();

void main() {
  pos = _pos[0];

  emit(0);
  emit(1);
  emit(3);
  emit(2);

  emit(6);
  emit(7);
  emit(5);
  emit(4);

  emit(0);
  EndPrimitive();

  emit(0);
  emit(2);
  EndPrimitive();

  emit(1);
  emit(5);
  EndPrimitive();

  emit(3);
  emit(7);
  EndPrimitive();

  emit(4);
  emit(6);
  EndPrimitive();
}

